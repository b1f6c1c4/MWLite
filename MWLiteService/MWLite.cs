using System;
using System.ServiceProcess;

namespace MWLiteService
{
    public partial class MWLite : ServiceBase
    {
        private static WebApp m_App;

        public MWLite() { InitializeComponent(); }

        protected override void OnStart(string[] args)
        {
            Program.ServiceLog("Starting");
            m_App = new WebApp(Environment.ProcessorCount);
            Program.ServiceLog("Started");
        }

        protected override void OnStop()
        {
            Program.ServiceLog("Stopping");
            m_App = null;
            Program.ServiceLog("Stopped");
        }
    }
}
