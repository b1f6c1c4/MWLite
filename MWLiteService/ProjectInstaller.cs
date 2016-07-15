using System.ComponentModel;
using System.Configuration.Install;

namespace MWLiteService
{
    [RunInstaller(true)]
    public partial class ProjectInstaller : Installer
    {
        public ProjectInstaller() { InitializeComponent(); }
    }
}
