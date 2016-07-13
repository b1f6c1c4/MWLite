using System;
using System.Windows.Forms;

namespace MWLiteUI
{
    public partial class FrmMain : Form
    {
        public FrmMain()
        {
            InitializeComponent();

            UpdateState();
        }

        private void button1_Click(object sender, EventArgs e) => UpdateState();

        private void UpdateState()
        {
            Program.TheCore.UpdateWorkerStates();

            listBox1.Items.Clear();
            foreach (var state in Program.TheCore.WorkerStates)
                listBox1.Items.Add(state.ToString());
        }
    }
}
