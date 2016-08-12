using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace AsmUpdater
{
    public delegate void NamedEventHandler(string name);

    internal class BundleUpdater : IEnumerable<string>
    {
        public event NamedEventHandler OnUpdating;
        public event NamedEventHandler OnNoUpdate;
        public event ExceptionEventHandler OnUpdateException;
        public event EventHandler OnStop;

        private bool m_Error;

        private readonly string m_BaseUri;
        private readonly string m_BaseDirectory;

        private readonly List<Updater> m_Targets;

        public BundleUpdater(string baseUri, string baseDirectory)
        {
            m_Error = false;

            m_BaseUri = baseUri;
            m_BaseDirectory = baseDirectory;

            m_Targets = new List<Updater>();
        }

        public void Add(string name) => m_Targets.Add(MakeTarget(name));

        public void Clear() => m_Targets.Clear();

        private Updater MakeTarget(string name)
        {
            var updater = new Updater(m_BaseUri + name, m_BaseDirectory, name);
            updater.OnUpdating += stream =>
                                  {
                                      OnStop?.Invoke();
                                      OnUpdating?.Invoke(name);
                                  };
            updater.OnNoUpdate += () => OnNoUpdate?.Invoke(name);
            updater.OnUpdateException += e =>
                                         {
                                             m_Error = true;
                                             OnUpdateException?.Invoke(e);
                                         };
            return updater;
        }

        public bool ForceUpdate()
        {
            m_Error = false;
            var updated = m_Targets.Aggregate(false, (u, target) => u | target.Update());
            return updated && !m_Error;
        }

        public IEnumerator<string> GetEnumerator() => m_Targets.Select(u => u.Name).GetEnumerator();
        IEnumerator IEnumerable.GetEnumerator() => m_Targets.Select(u => u.Name).GetEnumerator();
    }
}
