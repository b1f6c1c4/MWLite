using System;
using System.Configuration.Install;
using System.IO;
using System.Reflection;
using System.ServiceProcess;

namespace MWLiteService
{
    internal static class Program
    {
        private static readonly string LogFilePath = AppDomain.CurrentDomain.BaseDirectory + "/MWLiteService.log";

        private static readonly object Lock = new object();
        public static void ServiceLog(string str)
        {
            lock (Lock)
                try
                {
                    File.AppendAllText(LogFilePath, $"{DateTime.Now:O}: {str}{Environment.NewLine}");
                }
                catch (Exception)
                {
                    // ignored
                }
        }

        /// <summary>
        ///     应用程序的主入口点。
        /// </summary>
        private static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += CurrentDomainUnhandledException;

            if (Environment.UserInteractive)
            {
                var parameter = string.Concat(args);
                ServiceLog($"UserInteractive: {parameter}");
                switch (parameter)
                {
                    case "--install":
                        ManagedInstallerClass.InstallHelper(new[] { Assembly.GetExecutingAssembly().Location });
                        break;
                    case "--uninstall":
                        ManagedInstallerClass.InstallHelper(new[] { "/u", Assembly.GetExecutingAssembly().Location });
                        break;
                }
                return;
            }

            var servicesToRun = new ServiceBase[] { new MWLite() };
            ServiceBase.Run(servicesToRun);
        }

        private static void CurrentDomainUnhandledException(object sender, UnhandledExceptionEventArgs e)
            =>
                ServiceLog(
                           ((Exception)e.ExceptionObject).Message +
                           ((Exception)e.ExceptionObject).InnerException.Message);
    }
}
