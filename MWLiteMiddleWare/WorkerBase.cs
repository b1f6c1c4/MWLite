using System;
using System.Runtime.InteropServices;
using System.Threading;

namespace MWLiteMiddleWare
{
    internal abstract class WorkerBase : IDisposable
    {
        private bool m_Disposed;
        protected readonly object Lock = new object();
        protected IntPtr TheWorker;
        private readonly Thread m_Thread;
        protected readonly DbHelper Db;
        private readonly CancellationTokenSource m_Cancellation;
        private CancellationToken m_Token;

        protected WorkerBase(DbHelper db)
        {
            Db = db;
            m_Cancellation = new CancellationTokenSource();
            m_Token = m_Cancellation.Token;
            m_Thread = new Thread(WorkerThreadEntry) { Name = "MWWorker" };
            m_Thread.Start();
        }

        public event ExceptionEventHandler OnException;

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

        ~WorkerBase() { Dispose(false); }

        private void WorkerThreadEntry()
        {
            while (!m_Token.IsCancellationRequested)
                try
                {
                    var work = GetWorkLoad();
                    var result = Run(work);
                    Put(work, result);
                }
                catch (Exception e)
                {
                    Db.PutException(e.ToString());
                    OnException?.Invoke(e);
                }
        }

        protected abstract void Put(WorkingConfig work, long[] result);

        protected abstract WorkingConfig GetWorkLoad();

        private long[] Run(WorkingConfig work)
        {
            long[] result;
            try
            {
                if (work.Config.Width <= 0)
                    throw new ArgumentException(@"Width", nameof(work.Config));
                if (work.Config.Height <= 0)
                    throw new ArgumentException(@"Height", nameof(work.Config));
                if (work.Config.TotalMines < 0)
                    throw new ArgumentException(@"TotalMines", nameof(work.Config));

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
                ptr = DllWrapper.Run(TheWorker, out len);

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

        protected abstract void Make(WorkingConfig cfg);

        public void CancelCurrent()
        {
            if (TheWorker == IntPtr.Zero)
                return;

            lock (Lock)
                if (TheWorker != IntPtr.Zero)
                {
                    DllWrapper.CancelWorker(TheWorker);
                    DllWrapper.DisposeWorker(TheWorker);
                }
        }

        public void Join()
        {
            m_Cancellation.Cancel();
            CancelCurrent();
            m_Thread.Join();
        }
    }
}
