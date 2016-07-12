using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace MWLiteUI
{
    internal static class Program
    {
        public static Core TheCore;

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

            Application.Run(new FrmMain());
        }
    }
}
