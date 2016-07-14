using System;
using System.Collections.Generic;
using System.Threading;
using System.Timers;
using Timer = System.Timers.Timer;

namespace MWLiteUI
{
    public sealed class Core : IDisposable
    {
        private bool m_Disposed;

        private readonly Timer m_Timer;

        public long Speed { get; private set; }
        public long RestGame { get { return m_RestGame; } private set { m_RestGame = value; } }
        public double RestTime { get; private set; }

        public Core(int numWorkers)
        {
            DllWrapper.CreateWorkers(numWorkers);

            WorkerStates = new List<WorkerState>();

            Speed = 0;
            m_Timer = new Timer(1000)
                          {
                              AutoReset = true
                          };
            m_Timer.Elapsed += Timer_Elapsed;
            m_Timer.Start();

            UpdateWorkerStates();
        }

        private void Timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            Speed = (long)DllWrapper.ResetCounter();
            Interlocked.Add(ref m_RestGame, -Speed);
            RestTime = (double)Interlocked.Read(ref m_RestGame) / Speed;
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
                DllWrapper.RemoveWorkers();

            m_Disposed = true;
        }

        ~Core() { Dispose(false); }

        public readonly List<WorkerState> WorkerStates;
        private long m_RestGame;

        public void UpdateWorkerStates()
        {
            WorkerStates.Clear();
            var num = DllWrapper.GetNumWorkers();
            for (var i = 0; i < num; i++)
                WorkerStates.Add(DllWrapper.GetWorkerState(i));
        }

        public void Schedule(Configuration config, ulong repetition, ulong saveInterval)
        {
            if (config.Width <= 0)
                throw new ArgumentException(@"Width", nameof(config));
            if (config.Height <= 0)
                throw new ArgumentException(@"Height", nameof(config));
            if (config.UseTotalMines &&
                config.TotalMines < 0)
                throw new ArgumentException(@"TotalMines", nameof(config));
            if (!config.UseTotalMines &&
                (config.Probability < 0 || config.Probability > 1))
                throw new ArgumentException(@"Probability", nameof(config));

            Interlocked.Add(ref m_RestGame, (long)repetition);
            DllWrapper.Schedule(config, repetition, saveInterval);
        }

        public void Cancel()
        {
            UpdateWorkerStates();
            DllWrapper.EmptyQueue();
            for (var i = 0; i < WorkerStates.Count; i++)
                DllWrapper.CancelWorker(i);
            Interlocked.Exchange(ref m_RestGame, 0);
        }
    }
}
