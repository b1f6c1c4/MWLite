using System;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;

namespace AsmUpdater
{
    public class MW : IDisposable
    {
        private const string BaseUri = "https://b1f6c1c4.info/raw/MWLite/";
        private const string IP = "IPAddress.txt";
        private const string MineSweeper = "MWLite.dll";
        private const string MiddleWare = "MWLiteMiddleWare.dll";

        private readonly string[] m_AllDownloads =
            {
                IP,
                MineSweeper,
                MiddleWare,
                "HttpServer.dll",
                "Newtonsoft.Json.dll",
                "ServiceStack.Common.dll",
                "ServiceStack.Interfaces.dll",
                "ServiceStack.Redis.dll",
                "ServiceStack.Text.dll"
            };

        public delegate void LogEventHandler(string log);

        public event LogEventHandler OnLog;

        private bool m_Disposed;
        private readonly BundleUpdater m_Bundle;
        private AppDomain m_Domain;
        private object m_Facade;

        private static string BaseDirectory => Path.Combine(AppDomain.CurrentDomain.BaseDirectory, @"program\");

        public MW()
        {
            Directory.CreateDirectory(BaseDirectory);

            m_Bundle = new BundleUpdater(BaseUri, BaseDirectory, m_AllDownloads);

            m_Bundle.OnUpdating += name => OnLog?.Invoke($"Updating: {name}");
            m_Bundle.OnNoUpdate += name => OnLog?.Invoke($"Skipping: {name}");
            m_Bundle.OnUpdateException += e => OnLog?.Invoke($"Update error: {e}");
            m_Bundle.OnStart += Launch;
            m_Bundle.OnStop += Terminate;
        }

        public void Run() =>
            Task.Run(
                     () =>
                     {
                         while (true)
                             try
                             {
                                 OnLog?.Invoke("Start regular update");
                                 m_Bundle.ForceUpdate();
                                 OnLog?.Invoke("End regular update");
                                 Launch();

                                 Thread.Sleep(60000);
                             }
                             catch (Exception e)
                             {
                                 OnLog?.Invoke($"Unhandled error: {e}");
                             }
                         // ReSharper disable once FunctionNeverReturns
                     });

        private void Launch()
        {
            if (m_Facade != null)
                return;

            try
            {
                OnLog?.Invoke("Launching ...");
                var host = GetHost();
                m_Domain = AppDomain
                    .CreateDomain(
                                  "MWDomain",
                                  AppDomain.CurrentDomain.Evidence,
                                  BaseDirectory,
                                  BaseDirectory,
                                  false);
                m_Facade = m_Domain
                    .CreateInstanceFromAndUnwrap(
                                                 Path.Combine(BaseDirectory, MiddleWare),
                                                 "MWLiteMiddleWare.Facade",
                                                 false,
                                                 BindingFlags.Default,
                                                 null,
                                                 new object[] { host },
                                                 CultureInfo.InvariantCulture,
                                                 null);
                OnLog?.Invoke("Launched");
            }
            catch (Exception e)
            {
                OnLog?.Invoke($"Launch error: {e}");
            }
        }

        private void Terminate()
        {
            if (m_Facade == null)
                return;

            OnLog?.Invoke("Terminating ...");
            //m_Facade.Dispose();
            m_Facade = null;
            AppDomain.Unload(m_Domain);
            m_Domain = null;
            OnLog?.Invoke("Terminated");
        }

        private string GetHost()
        {
            var host = File.ReadAllText(Path.Combine(BaseDirectory, IP)).Trim();
            OnLog?.Invoke($"The host is: {host}");
            return host;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (m_Disposed)
                return;

            if (disposing)
                if (m_Facade != null)
                    ; //m_Facade.Dispose();

            m_Disposed = true;
        }

        ~MW() { Dispose(false); }
    }
}
