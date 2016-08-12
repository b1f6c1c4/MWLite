using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace AsmUpdater
{
    public delegate void NamedEventHandler(string name);

    internal class BundleUpdater
    {
        public event NamedEventHandler OnUpdating;
        public event NamedEventHandler OnNoUpdate;
        public event ExceptionEventHandler OnUpdateException;
        public event EventHandler OnStart;
        public event EventHandler OnStop;

        private bool m_Stopped;
        private bool m_Error;

        private sealed class Target
        {
            // ReSharper disable once MemberCanBePrivate.Local
            public readonly string Name;

            public readonly Updater Updater;

            private readonly string BaseDirectory;

            public Target(string name, string baseDirectory, Updater updater)
            {
                Name = name;
                Updater = updater;
                BaseDirectory = baseDirectory;
                Updater.OnChecking += Check;
            }

            private DateTime? Check()
            {
                if (!File.Exists(Path))
                    return null;
                return File.GetLastWriteTime(Path);
            }

            public void Write(Stream stream)
            {
                using (var file = File.Create(Path))
                    stream.CopyTo(file);
            }

            private string Path => System.IO.Path.Combine(BaseDirectory, Name);
        }

        private readonly List<Target> m_Targets;

        public BundleUpdater(string baseUri, string baseDirectory, IEnumerable<string> names)
        {
            m_Stopped = false;
            m_Error = false;

            m_Targets = new List<Target>();
            foreach (var name in names)
                m_Targets.Add(MakeTarget(baseUri, baseDirectory, name));
        }

        private Target MakeTarget(string baseUri, string baseDirectory, string name)
        {
            var updater = new Updater(baseUri + name);
            var target = new Target(name, baseDirectory, updater);
            updater.OnUpdating += stream =>
                                  {
                                      Stop();
                                      OnUpdating?.Invoke(name);
                                      target.Write(stream);
                                  };
            updater.OnNoUpdate += () => OnNoUpdate?.Invoke(name);
            updater.OnUpdateException += e =>
                                         {
                                             m_Error = true;
                                             OnUpdateException?.Invoke(e);
                                         };
            return target;
        }

        private void Stop()
        {
            if (m_Stopped)
                return;

            OnStop?.Invoke();
            m_Stopped = true;
        }

        public bool ForceUpdate()
        {
            m_Error = false;
            var updated = m_Targets.Aggregate(false, (u, target) => u | target.Updater.ForceUpdate());
            if (!updated || m_Error)
                return false;

            OnStart?.Invoke();
            m_Stopped = false;
            return true;
        }
    }
}
