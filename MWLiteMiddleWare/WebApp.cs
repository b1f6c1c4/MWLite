using System;
using System.Net;
using CSharpUtil;

namespace MWLiteMiddleWare
{
    public class WebApp
    {
        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly HttpServer m_Http;
        private Core m_Core;

        public WebApp()
        {
            m_Http = new HttpServer(IPAddress.Any, 23456);
            m_Http.OnHttpRequest += HttpRequest;
        }

        public void Run() => m_Http.Start();

        private HttpResponse PutCancel()
        {
            m_Core.CancelCurrent();
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
                            return m_Core == null
                                       ? new HttpResponse { ResponseCode = 404 }
                                       : new HttpResponse { ResponseCode = 202 };
                        default:
                            throw new HttpException(404);
                    }
                case "POST":
                    switch (request.BaseUri)
                    {
                        case "/create":
                            return PutCreate(request);
                        case "/cancel":
                            return PutCancel();
                        case "/dispose":
                            return PutDispose();
                        default:
                            throw new HttpException(404);
                    }
                default:
                    throw new HttpException(405);
            }
        }

        private HttpResponse PutDispose()
        {
            m_Core.Dispose();
            m_Core = null;
            return new HttpResponse { ResponseCode = 202 };
        }

        private HttpResponse PutCreate(HttpRequest request)
        {
            var db = request.ReadToEnd();
            m_Core = new Core(Environment.ProcessorCount, db);
            return new HttpResponse { ResponseCode = 202 };
        }
    }
}
