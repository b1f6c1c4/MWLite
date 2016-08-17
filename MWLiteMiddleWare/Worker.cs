using System;
using System.Runtime.InteropServices;
using System.Threading;

namespace MWLiteMiddleWare
{
    internal class Worker : IDisposable
    {
        public event ExceptionEventHandler OnException;

        private bool m_Disposed;

        private readonly object m_Lock = new object();

        private IntPtr m_Worker;

        private readonly Thread m_Thread;

        private readonly DbHelper m_Db;

        private readonly CancellationTokenSource m_Cancellation;
        private readonly CancellationToken m_Token;

        public Worker(DbHelper db)
        {
            m_Db = db;
            m_Cancellation = new CancellationTokenSource();
            m_Token = m_Cancellation.Token;
            m_Thread = new Thread(WorkerThreadEntry) { Name = "MWWorker" };
            m_Thread.Start();
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
                Join();

            m_Disposed = true;
        }

        ~Worker() { Dispose(false); }

        private void WorkerThreadEntry()
        {
            while (!m_Token.IsCancellationRequested)
                try
                {
                    var work = m_Db.GetWorkLoad();
                    var result = Run(work);
                    m_Db.PutResult(work.Config, result);
                }
                catch (Exception e)
                {
                    m_Db.PutException(e.ToString());
                    OnException?.Invoke(e);
                }
        }

        private long[] Run(WorkingConfig work)
        {
            long[] result;
            try
            {
                Make(work);
                result = GetResult();
            }
            finally
            {
                CancelCurrent();
            }
            return result;
        }

        private long[] GetResult()
        {
            long[] result;
            var ptr = IntPtr.Zero;
            try
            {
                ulong len;
                ptr = DllWrapper.Run(m_Worker, out len);

                result = new long[len];
                Marshal.Copy(ptr, result, 0, (int)len);
            }
            finally
            {
                if (ptr != IntPtr.Zero)
                    DllWrapper.DisposeResult(ptr);
            }
            return result;
        }

        private void Make(WorkingConfig cfg)
        {
            if (cfg.Config.Width <= 0)
                throw new ArgumentException(@"Width", nameof(cfg.Config));
            if (cfg.Config.Height <= 0)
                throw new ArgumentException(@"Height", nameof(cfg.Config));
            if (cfg.Config.TotalMines < 0)
                throw new ArgumentException(@"TotalMines", nameof(cfg.Config));

            lock (m_Lock)
                m_Worker = DllWrapper.MakeWorker(cfg.Config, cfg.Repetition);
        }

        public void CancelCurrent()
        {
            if (m_Worker == IntPtr.Zero)
                return;

            lock (m_Lock)
                if (m_Worker != IntPtr.Zero)
                    DllWrapper.CancelWorker(m_Worker);
        }

        public void Join()
        {
            m_Cancellation.Cancel();
            CancelCurrent();
            m_Thread.Join();
        }
    }
}
