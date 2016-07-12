using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Net;
using Newtonsoft.Json;

namespace MWLiteUI
{
    public class WebApp
    {
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

        private HttpResponse HttpRequest(HttpRequest request)
        {
            if (request.Method == "GET")
            {
                if (request.Uri != "/")
                    throw new HttpException(404);

                Program.TheCore.UpdateWorkerStates();

                return GenerateHttpResponse(JsonConvert.SerializeObject(Program.TheCore.WorkerStates));
            }
            if (request.Method == "POST")
            {
                // TODO
            }
            throw new HttpException(405);
        }
    }
}
