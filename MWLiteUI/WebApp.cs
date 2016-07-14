using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Net;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace MWLiteUI
{
    public class WebApp
    {
        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly HttpServer m_Http;

        public WebApp()
        {
            m_Http = new HttpServer(IPAddress.Any, 23456);
            m_Http.OnHttpRequest += HttpRequest;
        }

        private static HttpResponse GetWorkerStates()
        {
            Program.TheCore.UpdateWorkerStates();
            return HttpServer.GenerateHttpResponse(JsonConvert.SerializeObject(Program.TheCore.WorkerStates));
        }

        private static HttpResponse GetSpeed()
        {
            var json =
                new JObject
                    {
                        { "Speed", Program.TheCore.Speed },
                        { "RestGame", Program.TheCore.RestGame },
                        { "RestTime", Program.TheCore.RestTime }
                    };
            return HttpServer.GenerateHttpResponse(json.ToString());
        }

        private static HttpResponse GetResult(HttpRequest request)
        {
            var config = JsonConvert.DeserializeObject<Configuration>(HttpServer.ReadToEnd(request));
            var res = Core.GatherResult(config);
            return HttpServer.GenerateHttpResponse(JsonConvert.SerializeObject(res));
        }

        private static HttpResponse PutSchedule(HttpRequest request)
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
                Program.TheCore.Schedule(c, rep, sav);

            return new HttpResponse { ResponseCode = 202 };
        }

        private static HttpResponse PutCancel()
        {
            Program.TheCore.Cancel();
            return new HttpResponse { ResponseCode = 202 };
        }

        private static HttpResponse HttpRequest(HttpRequest request)
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
