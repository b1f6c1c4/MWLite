using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace MWLiteUI
{
    internal static class Program
    {
        // ReSharper disable MemberCanBePrivate.Global
        // ReSharper disable NotAccessedField.Global
        public static Core TheCore;
        public static WebApp TheWeb;
        // ReSharper restore MemberCanBePrivate.Global
        // ReSharper restore NotAccessedField.Global

        [DllImport("user32.dll")]
        private static extern bool SetProcessDPIAware();

        /// <summary>
        ///     应用程序的主入口点。
        /// </summary>
        [STAThread]
        private static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            SetProcessDPIAware();
            TheCore = new Core(Environment.ProcessorCount);
            TheWeb = new WebApp();

            Application.Run(new FrmMain());
        }
    }
}
