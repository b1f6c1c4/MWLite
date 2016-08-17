using System;
using System.Collections.Generic;

namespace MWLiteMiddleWare
{
    public delegate void ExceptionEventHandler(Exception e);

    internal sealed class Core : IDisposable
    {
        public event ExceptionEventHandler OnException;

        private bool m_Disposed;
        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly DbHelper m_Db;
        private readonly List<WorkerBase> m_Workers;

        public Core(int numWorkers, string uri, Func<DbHelper, WorkerBase> creator)
        {
            m_Db = new DbHelper(uri);

            m_Workers = new List<WorkerBase>();
            for (var i = 0; i < numWorkers; i++)
            {
                var worker = creator(m_Db);
                worker.OnException += e => OnException?.Invoke(e);
                m_Workers.Add(worker);
            }
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
