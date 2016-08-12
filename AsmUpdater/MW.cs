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
#if DEBUG
        private const string BaseUri = "https://b1f6c1c4.info/raw/MWLite/debug/";
#else
        private const string BaseUri = "https://b1f6c1c4.info/raw/MWLite/";
#endif
        private const string Config = "config.txt";

        public delegate void LogEventHandler(string log);

        public event LogEventHandler OnLog;

        private bool m_Disposed;
        private readonly BundleUpdater m_BundleConfig;
        private readonly BundleUpdater m_BundleProgram;
        private AppDomain m_Domain;
        private object m_Facade;

        private string m_Host;
        private string m_AssemblyName;

        private static string BaseDirectory => Path.Combine(AppDomain.CurrentDomain.BaseDirectory, @"program\");

        private static string GetPath(string file) => Path.Combine(BaseDirectory, file);

        public MW()
        {
            Directory.CreateDirectory(BaseDirectory);

            m_BundleConfig = new BundleUpdater(BaseUri, BaseDirectory) { Config };

            m_BundleConfig.OnUpdating += name => OnLog?.Invoke($"Updating: {name}");
            m_BundleConfig.OnNoUpdate += name => OnLog?.Invoke($"Skipping: {name}");
            m_BundleConfig.OnUpdateException += e => OnLog?.Invoke($"Update error: {e}");
            m_BundleConfig.OnStop += Terminate;

            m_BundleProgram = new BundleUpdater(BaseUri, BaseDirectory);

            m_BundleProgram.OnUpdating += name => OnLog?.Invoke($"Updating: {name}");
            m_BundleProgram.OnNoUpdate += name => OnLog?.Invoke($"Skipping: {name}");
            m_BundleProgram.OnUpdateException += e => OnLog?.Invoke($"Update error: {e}");
            m_BundleProgram.OnStop += Terminate;
        }

        public void Run() =>
            Task.Run(
                     () =>
                     {
                         while (true)
                             try
                             {
                                 RegularUpdate();
                                 Launch();

                                 Thread.Sleep(600000);
                             }
                             catch (Exception e)
                             {
                                 OnLog?.Invoke($"Unhandled error: {e}");
                             }
                         // ReSharper disable once FunctionNeverReturns
                     });

        private void RegularUpdate()
        {
            OnLog?.Invoke("Start regular update");

            OnLog?.Invoke("Config:");
            m_BundleConfig.ForceUpdate();

            LoadConfig();

            if (!m_BundleProgram.ForceUpdate())
            {
                OnLog?.Invoke("Not changed; end regular update");
                return;
            }

            OnLog?.Invoke("Finished regular update");
        }

        private void LoadConfig()
        {
            OnLog?.Invoke("Loading config:");

            var cfg = File.ReadAllLines(GetPath(Config));
            if (cfg.Length < 2)
                throw new FormatException("Config file format error");

            m_BundleProgram.Clear();

            m_Host = cfg[0];
            m_AssemblyName = cfg[1];
            OnLog?.Invoke($"The host is {m_Host}");
            OnLog?.Invoke($"The assembly is {m_AssemblyName}");
            m_BundleProgram.Add(m_AssemblyName);

            for (var i = 2; i < cfg.Length; i++)
            {
                if (string.IsNullOrWhiteSpace(cfg[i]))
                    continue;
                m_BundleProgram.Add(cfg[i]);
                OnLog?.Invoke($"Add {cfg[i]} to dependencies");
            }

            OnLog?.Invoke("Done loading config");
        }

        private void Launch()
        {
            if (m_Facade != null)
                return;

            try
            {
                OnLog?.Invoke("Launching ...");
                m_Domain = AppDomain
                    .CreateDomain(
                                  "MWDomain",
                                  AppDomain.CurrentDomain.Evidence,
                                  BaseDirectory,
                                  BaseDirectory,
                                  false);
                m_Facade = m_Domain
                    .CreateInstanceFromAndUnwrap(
                                                 Path.Combine(BaseDirectory, m_AssemblyName),
                                                 "MWLiteMiddleWare.Facade",
                                                 false,
                                                 BindingFlags.Default,
                                                 null,
                                                 new object[] { m_Host },
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
            try
            {
                m_Facade = null;
                GC.Collect();
                AppDomain.Unload(m_Domain);
                m_Domain = null;
                OnLog?.Invoke("Terminated");
            }
            catch (Exception e)
            {
                OnLog?.Invoke($"Terminate error: {e}");
            }
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
                Terminate();

            m_Disposed = true;
        }

        ~MW() { Dispose(false); }
    }
}
