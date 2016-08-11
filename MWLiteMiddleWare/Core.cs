using System;
using System.Collections.Generic;

namespace MWLiteMiddleWare
{
    public sealed class Core : IDisposable
    {
        private bool m_Disposed;
        private readonly DbHelper m_Db;
        private readonly List<Worker> m_Workers;

        public Core(int numWorkers, string uri)
        {
            m_Db = new DbHelper(uri);

            m_Workers = new List<Worker>();
            for (var i = 0; i < numWorkers; i++)
                m_Workers.Add(new Worker(m_Db));
        }

        public void CancelCurrent()
        {
            foreach (var worker in m_Workers)
                worker.CancelCurrent();
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
                foreach (var worker in m_Workers)
                    worker.Join();

            m_Disposed = true;
        }

        ~Core() { Dispose(false); }
    }
}
