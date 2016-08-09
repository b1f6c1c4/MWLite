using System;
using System.Net;
using CSharpUtil;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace MWLiteMiddleWare
{
    public class WebApp
    {
        public delegate void LogEventHandler(string str);

        public event LogEventHandler Log;

        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly HttpServer m_Http;
        private readonly Core m_Core;

        public WebApp(int numWorkers)
        {
            m_Core = new Core(numWorkers);
            m_Http = new HttpServer(IPAddress.Any, 23456);
            m_Http.OnHttpRequest += HttpRequest;
        }

        public void Run() => m_Http.Start();

        private HttpResponse GetWorkerStates()
        {
            m_Core.UpdateWorkerStates();
            return HttpUtil.GenerateHttpResponse(JsonConvert.SerializeObject(m_Core.WorkerStates));
        }

        private HttpResponse GetSpeed()
        {
            var json =
                new JObject
                    {
                        { "Speed", m_Core.Speed },
                        { "RestGame", m_Core.RestGame },
                        { "RestTime", m_Core.RestTime }
                    };
            return HttpUtil.GenerateHttpResponse(json.ToString());
        }

        private static HttpResponse GetResult(HttpRequest request)
        {
            dynamic json = JsonConvert.DeserializeObject(request.ReadToEnd());
            LogicLevel level;
            var config = ParseConfig(json, out level);
            var res = Core.GatherResult(config, level);
            return HttpUtil.GenerateHttpResponse(JsonConvert.SerializeObject(res));
        }

        private static Configuration ParseConfig(dynamic json, out LogicLevel level)
        {
            var config = new Configuration
                             {
                                 Width = json.Width,
                                 Height = json.Height,
                                 UseTotalMines = json.UseTotalMines,
                                 TotalMines = json.TotalMines,
                                 Probability = json.Probability,
                                 NotRigorous = json.NotRigorous
                             };
            level = json.Logic;
            return config;
        }

        private HttpResponse PutSchedule(HttpRequest request)
        {
            if (request.Parameters == null)
                throw new HttpException(404);
            if (!request.Parameters.ContainsKey("r"))
                throw new HttpException(404);
            if (!request.Parameters.ContainsKey("s"))
                throw new HttpException(404);
            if (!request.Header.ContainsKey("Content-Length"))
                throw new HttpException(411);

            dynamic json = JsonConvert.DeserializeObject(request.ReadToEnd());
            var rep = Convert.ToUInt64(request.Parameters["r"]);
            var sav = Convert.ToUInt64(request.Parameters["s"]);

            foreach (var j in json)
            {
                LogicLevel l;
                var c = ParseConfig(j, out l);
                Log?.Invoke($"{Core.Hash(c, l)} r={rep} s={sav}");
                m_Core.Schedule(c, l, rep, sav);
            }

            return new HttpResponse { ResponseCode = 202 };
        }

        private HttpResponse PutCancel()
        {
            m_Core.Cancel();
            return new HttpResponse { ResponseCode = 202 };
        }

        private HttpResponse HttpRequest(HttpRequest request)
        {
            switch (request.Method)
            {
                case "GET":
                    switch (request.BaseUri)
                    {
                        case "/":
                            return GetWorkerStates();
                        case "/speed":
                            return GetSpeed();
                        default:
                            throw new HttpException(404);
                    }
                case "POST":
                    switch (request.BaseUri)
                    {
                        case "/schedule":
                            return PutSchedule(request);
                        case "/cancel":
                            return PutCancel();
                        case "/result":
                            return GetResult(request);
                        default:
                            throw new HttpException(404);
                    }
                default:
                    throw new HttpException(405);
            }
        }
    }
}
