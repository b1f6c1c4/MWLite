using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using System.Timers;
using Timer = System.Timers.Timer;

namespace MWLiteMiddleWare
{
    public sealed class Core : IDisposable
    {
        private bool m_Disposed;

        // ReSharper disable once PrivateFieldCanBeConvertedToLocalVariable
        private readonly Timer m_Timer;

        public long Speed { get; private set; }
        public long RestGame => m_RestGame;
        public double RestTime { get; private set; }

        public Core(int numWorkers)
        {
            DllWrapper.SetWorkingDirectory(AppDomain.CurrentDomain.BaseDirectory);
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

        public void Schedule(Configuration config, LogicLevel level, ulong repetition, ulong saveInterval)
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
            DllWrapper.Schedule(config, level, repetition, saveInterval);
        }

        public void Cancel()
        {
            UpdateWorkerStates();
            DllWrapper.EmptyQueue();
            for (var i = 0; i < WorkerStates.Count; i++)
                DllWrapper.CancelWorker(i);
            Interlocked.Exchange(ref m_RestGame, 0);
        }

        public static string Hash(Configuration config, LogicLevel level)
        {
            var sb = new StringBuilder();
            switch (level)
            {
                case LogicLevel.ZeroLogic:
                    sb.Append("ZL");
                    break;
                case LogicLevel.PassiveLogic:
                    sb.Append("PL");
                    break;
                case LogicLevel.SingleLogic:
                    sb.Append("SL");
                    break;
                case LogicLevel.SingleLogicExtended:
                    sb.Append("SLE");
                    break;
                case LogicLevel.DoubleLogic:
                    sb.Append("DL");
                    break;
                case LogicLevel.DoubleLogicExtended:
                    sb.Append("DLE");
                    break;
                case LogicLevel.FullLogic:
                    sb.Append("FL");
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
            sb.Append($" {config.Width}-{config.Height}-");
            sb.Append(config.UseTotalMines ? $"T{config.TotalMines}" : $"P{config.Probability:R}");
            if (config.NotRigorous)
                sb.Append(" NR");
            return sb.ToString();
        }

        public static List<ulong> GatherResult(Configuration config, LogicLevel level)
        {
            var db = AppDomain.CurrentDomain.BaseDirectory + @"db\" + Hash(config, level);

            if (!Directory.Exists(db))
                return null;

            var lst = new List<ulong>();
            if (File.Exists(db + ".log"))
                using (var sr = new StreamReader(db + ".log"))
                    while (!sr.EndOfStream)
                    {
                        var str = sr.ReadLine();
                        lst.Add(Convert.ToUInt64(str));
                    }

            var flag = false;
            foreach (var file in Directory.EnumerateFiles(db + @"\", @"*.log"))
                try
                {
                    var id = 0;
                    using (var sr = new StreamReader(file))
                        while (!sr.EndOfStream)
                        {
                            var str = sr.ReadLine();
                            var cnt = Convert.ToUInt64(str);
                            if (id < lst.Count)
                                lst[id] += cnt;
                            else
                                lst.Add(cnt);
                            id++;
                        }
                    File.Move(file, file + ".bak");
                    flag = true;
                }
                catch (Exception)
                {
                    // ignore
                }

            if (!flag)
                return lst;

            using (var sw = new StreamWriter(db + ".log"))
                foreach (var cnt in lst)
                    sw.WriteLine(cnt);

            return lst;
        }
    }
}
