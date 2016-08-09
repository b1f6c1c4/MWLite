using System;
using MWLiteMiddleWare;

namespace MWLiteStandalone
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var app = new WebApp(Environment.ProcessorCount);
            app.Run();
            Console.Read();
        }
    }
}
