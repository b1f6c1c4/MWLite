using System.Runtime.InteropServices;

namespace MWLiteUI
{
    public enum WorkerState
    {
        Idle = 0,
        Running = 1,
        Cancelling = 2,
        Finished = 3,
        Quitting = 4
    };

    public static class DllWrapper
    {
        [DllImport("MWLite.dll")]
        public static extern void CreateWorkers(int numWorkers);

        [DllImport("MWLite.dll")]
        public static extern long GetNumWorkers();

        [DllImport("MWLite.dll")]
        public static extern void Schedule(Configuration config, long repetition, long saveInterval);

        [DllImport("MWLite.dll")]
        public static extern void CancelWorker(int id);

        [DllImport("MWLite.dll")]
        public static extern WorkerState GetWorkerState(int id);

        [DllImport("MWLite.dll")]
        public static extern void RemoveWorkers();
    }
}
