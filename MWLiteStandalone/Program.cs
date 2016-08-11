using System;
using MWLiteMiddleWare;

namespace MWLiteStandalone
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var app = new WebApp();
            app.Run();
            Console.Read();
        }
    }
}
