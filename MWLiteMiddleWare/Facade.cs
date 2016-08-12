using System;

namespace MWLiteMiddleWare
{
    public delegate void StringExceptionEventHandler(string exception);

    [Serializable]
    // ReSharper disable once UnusedMember.Global
    public class Facade : MarshalByRefObject, IDisposable
    {
        // ReSharper disable once EventNeverSubscribedTo.Global
        public event StringExceptionEventHandler OnException;

        private bool m_Disposed;

        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly Core m_Core;

        public Facade(string host)
        {
            try
            {
                m_Core = new Core(Environment.ProcessorCount, host);
                m_Core.OnException += e => OnException?.Invoke(e.ToString());
            }
            catch (Exception e)
            {
                OnException?.Invoke(e.ToString());
            }
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
                m_Core.Dispose();

            m_Disposed = true;
        }

        ~Facade() { Dispose(false); }
    }
}
