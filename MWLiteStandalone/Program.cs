using System;
using AsmUpdater;

namespace MWLiteStandalone
{
    internal class Program
    {
        private static MW m_App;

        private static void Main(string[] args)
        {
            m_App = new MW();
            m_App.OnLog += Console.WriteLine;
            m_App.Run();
            Console.Read();
        }
    }
}
