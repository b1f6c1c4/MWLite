using System;
using System.ServiceProcess;
using AsmUpdater;

namespace MWLiteService
{
    public partial class MWLite : ServiceBase
    {
        private static MW m_App;

        public MWLite() { InitializeComponent(); }

        protected override void OnStart(string[] args)
        {
            Program.ServiceLog("Starting");
            m_App = new MW(Environment.ProcessorCount, false, true);
            m_App.OnLog += Program.ServiceLog;
            m_App.Run();
            Program.ServiceLog("Started");
        }

        protected override void OnStop()
        {
            Program.ServiceLog("Stopping");
            Program.ServiceLog("Stopped");
        }
    }
}
