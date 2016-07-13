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

        public void Schedule(Configuration config, ulong repetition, ulong saveInterval)
        {
            if (config.Width <= 0)
                throw new ArgumentException(@"Width", nameof(config));
            if (config.Height <= 0)
                throw new ArgumentException(@"Height", nameof(config));
            if (config.UseTotalMines &&
                config.TotalMines < 0)
                throw new ArgumentException(@"TotalMines", nameof(config));
            if (!config.UseTotalMines &&
                (config.Probability < 0 || config.Probability > 1))
                throw new ArgumentException(@"Probability", nameof(config));

            DllWrapper.Schedule(config, repetition, saveInterval);
        }

        public void Cancel(int id)
        {
            UpdateWorkerStates();
            DllWrapper.CancelWorker(id);
        }
        public void Cancel()
        {
            UpdateWorkerStates();
            for (var i = 0; i < WorkerStates.Count; i++)
                DllWrapper.CancelWorker(i);
        }

    }
}
