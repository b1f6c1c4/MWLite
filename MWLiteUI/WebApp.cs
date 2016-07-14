using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Net;
using System.Text;
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

        private static HttpResponse GenerateHttpResponse(string str, string contentType = "text/json")
        {
            var stream = new MemoryStream();
            var sw = new StreamWriter(stream);
            sw.Write(str);
            sw.Flush();
            stream.Position = 0;
            return
                new HttpResponse
                    {
                        ResponseCode = 200,
                        Header =
                            new Dictionary<string, string>
                                {
                                    { "Content-Type", contentType },
                                    { "Content-Length", stream.Length.ToString(CultureInfo.InvariantCulture) }
                                },
                        ResponseStream = stream
                    };
        }

        private static HttpResponse HttpRequest(HttpRequest request)
        {
            switch (request.Method)
            {
                case "GET":
                    switch (request.BaseUri)
                    {
                        case "/":
                            Program.TheCore.UpdateWorkerStates();
                            return GenerateHttpResponse(JsonConvert.SerializeObject(Program.TheCore.WorkerStates));

                        case "/speed":
                            var json =
                                new JObject
                                    {
                                        { "Speed", Program.TheCore.Speed },
                                        { "RestGame", Program.TheCore.RestGame },
                                        { "RestTime", Program.TheCore.RestTime },
                                    };

                            return GenerateHttpResponse(json.ToString());

                        default:
                            throw new HttpException(404);
                    }
                case "POST":
                    switch (request.BaseUri)
                    {
                        case "/schedule":
                            if (request.Parameters == null)
                                throw new HttpException(404);
                            if (!request.Parameters.ContainsKey("r"))
                                throw new HttpException(404);
                            if (!request.Parameters.ContainsKey("s"))
                                throw new HttpException(404);
                            if (!request.Header.ContainsKey("Content-Length"))
                                throw new HttpException(411);

                            var len = Convert.ToInt32(request.Header["Content-Length"]);
                            if (len > 1048576)
                                throw new HttpException(413);
                            var buff = new byte[len];
                            if (len > 0)
                                request.RequestStream.Read(buff, 0, len);
                            var str = Encoding.UTF8.GetString(buff);

                            var config = JsonConvert.DeserializeObject<Configuration[]>(str);
                            var rep = Convert.ToUInt64(request.Parameters["r"]);
                            var sav = Convert.ToUInt64(request.Parameters["s"]);

                            foreach (var c in config)
                                Program.TheCore.Schedule(c, rep, sav);

                            return new HttpResponse { ResponseCode = 202 };

                        case "/cancel":
                            Program.TheCore.Cancel();

                            return new HttpResponse { ResponseCode = 202 };

                        default:
                            throw new HttpException(404);
                    }
                default:
                    throw new HttpException(405);
            }
        }
    }
}
