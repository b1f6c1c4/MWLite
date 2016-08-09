using System;
using System.Net;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace MWLiteService
{
    public class WebApp
    {
        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly HttpServer m_Http;
        private readonly Core m_Core;

        public WebApp(int numWorkers)
        {
            m_Core = new Core(numWorkers);
            m_Http = new HttpServer(IPAddress.Any, 23456);
            m_Http.OnHttpRequest += HttpRequest;
        }

        private HttpResponse GetWorkerStates()
        {
            m_Core.UpdateWorkerStates();
            return HttpServer.GenerateHttpResponse(JsonConvert.SerializeObject(m_Core.WorkerStates));
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
            return HttpServer.GenerateHttpResponse(json.ToString());
        }

        private static HttpResponse GetResult(HttpRequest request)
        {
            var config = JsonConvert.DeserializeObject<Configuration>(HttpServer.ReadToEnd(request));
            var res = Core.GatherResult(config);
            return HttpServer.GenerateHttpResponse(JsonConvert.SerializeObject(res));
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

            var config = JsonConvert.DeserializeObject<Configuration[]>(HttpServer.ReadToEnd(request));
            var rep = Convert.ToUInt64(request.Parameters["r"]);
            var sav = Convert.ToUInt64(request.Parameters["s"]);

            foreach (var c in config)
            {
                Program.ServiceLog($"{Core.Hash(c)} r={rep} s={sav}");
                m_Core.Schedule(c, rep, sav);
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
