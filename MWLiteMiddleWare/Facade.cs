using System;

namespace MWLiteMiddleWare
{
    [Serializable]
    // ReSharper disable once UnusedMember.Global
    public class Facade : MarshalByRefObject, IDisposable
    {
        // ReSharper disable once EventNeverSubscribedTo.Global
        public event ExceptionEventHandler OnException;

        private bool m_Disposed;

        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private Core m_Core;

        public Facade(string host)
        {
            m_Core = new Core(Environment.ProcessorCount, host);
            m_Core.OnException += e => OnException?.Invoke(e);
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
