using System;
using System.ServiceProcess;
using System.Threading.Tasks;
using MWLiteMiddleWare;

namespace MWLiteService
{
    public partial class MWLite : ServiceBase
    {
        private static WebApp m_App;

        public MWLite() { InitializeComponent(); }

        protected override void OnStart(string[] args)
        {
            Program.ServiceLog("Starting");
            m_App = new WebApp();
            m_App.Run();
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
