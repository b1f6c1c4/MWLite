using System;
using System.IO;
using System.Net;
using System.Runtime.Remoting;

namespace AsmUpdater
{
    public delegate void UpdatingEventHandler(Stream stream);

    public delegate void EventHandler();

    public delegate void ExceptionEventHandler(Exception e);

    internal class Updater
    {
        public event UpdatingEventHandler OnUpdating;
        public event EventHandler OnNoUpdate;
        public event ExceptionEventHandler OnUpdateException;

        private readonly string m_Uri;

        private readonly string m_BaseDirectory;

        // ReSharper disable once MemberCanBePrivate.Local
        public readonly string Name;

        private DateTime? Check()
        {
            if (!File.Exists(CurrentPath))
                return null;
            return File.GetLastWriteTime(CurrentPath);
        }

        public void Write(Stream stream)
        {
            using (var file = File.Create(CurrentPath))
                stream.CopyTo(file);
        }

        private string CurrentPath => Path.Combine(m_BaseDirectory, Name);

        public Updater(string uri, string baseDirectory, string name)
        {
            m_Uri = uri;
            m_BaseDirectory = baseDirectory;
            Name = name;
        }

        public bool Update()
        {
            try
            {
                var req = WebRequest.CreateHttp(m_Uri);
                req.KeepAlive = true;
                var lmd = Check();
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

        private void GoUpdate(Stream stream)
        {
            OnUpdating?.Invoke(stream);
            Write(stream);
        }
    }
}
