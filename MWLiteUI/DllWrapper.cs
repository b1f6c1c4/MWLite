using System.Runtime.InteropServices;

namespace MWLiteUI
{
    public static class DllWrapper
    {
        [DllImport("MWLite.dll")]
        public static extern void CreateWorkers(int numWorkers);

        [DllImport("MWLite.dll")]
        public static extern void Schedule(Configuration config, long repetition, long saveInterval);

        [DllImport("MWLite.dll")]
        public static extern void RemoveWorkers();
    }
}
