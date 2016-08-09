using System.Runtime.InteropServices;

namespace MWLiteService
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
        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetWorkingDirectory(string path);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CreateWorkers(int numWorkers);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern long GetNumWorkers();

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Schedule(Configuration config, ulong repetition, ulong saveInterval);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CancelWorker(int id);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern WorkerState GetWorkerState(int id);

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void EmptyQueue();

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong ResetCounter();

        [DllImport("MWLite.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void RemoveWorkers();
    }
}
