using System;
using System.Windows.Forms;

namespace MWLiteUI
{
    internal static class Program
    {
        public static Core TheCore = null;

        /// <summary>
        ///     应用程序的主入口点。
        /// </summary>
        [STAThread]
        private static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new FrmMain());
        }
    }
}
