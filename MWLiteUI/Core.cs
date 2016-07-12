using System;

namespace MWLiteUI
{
    public sealed class Core : IDisposable
    {
        private bool m_Disposed;

        public Core(int numWorkers) { DllWrapper.CreateWorkers(numWorkers); }

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


        public void Schedule(Configuration config, long repetition)
        {
            DllWrapper.Schedule(config, repetition, 5);
        }
    }
}
