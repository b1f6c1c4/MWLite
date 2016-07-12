using System;
using System.Collections.Generic;

namespace MWLiteUI
{
    public sealed class Core : IDisposable
    {
        private bool m_Disposed;

        public Core(int numWorkers)
        {
            DllWrapper.CreateWorkers(numWorkers);
            WorkerStates = new List<WorkerState>();
            UpdateWorkerStates();
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
                DllWrapper.RemoveWorkers();

            m_Disposed = true;
        }

        ~Core() { Dispose(false); }

        public readonly List<WorkerState> WorkerStates;

        public void UpdateWorkerStates()
        {
            WorkerStates.Clear();
            var num = DllWrapper.GetNumWorkers();
            for (var i = 0; i < num; i++)
                WorkerStates.Add(DllWrapper.GetWorkerState(i));
        }

        public void Schedule(Configuration config, long repetition, long saveInterval)
            => DllWrapper.Schedule(config, repetition, saveInterval);
    }
}
