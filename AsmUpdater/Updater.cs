using System;
using System.IO;
using System.Net;
using System.Runtime.Remoting;

namespace AsmUpdater
{
    public delegate DateTime? CheckingEventHandler();

    public delegate void UpdatingEventHandler(Stream stream);

    public delegate void EventHandler();

    public delegate void ExceptionEventHandler(Exception e);

    internal class Updater
    {
        public event CheckingEventHandler OnChecking;
        public event UpdatingEventHandler OnUpdating;
        public event EventHandler OnNoUpdate;
        public event ExceptionEventHandler OnUpdateException;

        private readonly object m_Lock = new object();

        private readonly string m_Uri;

        public Updater(string uri) { m_Uri = uri; }

        public bool ForceUpdate()
        {
            lock (m_Lock)
                return Update();
        }

        private bool Update()
        {
            try
            {
                var req = WebRequest.CreateHttp(m_Uri);
                req.KeepAlive = true;
                var lmd = OnChecking?.Invoke();
                if (lmd != null)
                    req.IfModifiedSince = lmd.Value.ToUniversalTime();

                using (var res = req.GetResponse() as HttpWebResponse)
                {
                    if (res == null)
                        throw new RemotingException();

                    using (var stream = res.GetResponseStream())
                        GoUpdate(stream);
                    return true;
                }
            }
            catch (WebException we)
            {
                var hrsp = we.Response as HttpWebResponse;
                if (hrsp?.StatusCode != HttpStatusCode.NotModified)
                    OnUpdateException?.Invoke(we);
                else
                    OnNoUpdate?.Invoke();
            }
            catch (Exception e)
            {
                OnUpdateException?.Invoke(e);
            }
            return false;
        }

        private void GoUpdate(Stream stream) => OnUpdating?.Invoke(stream);
    }
}
