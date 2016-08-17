using System;
using System.Text;
using System.Threading;
using Newtonsoft.Json;
using StackExchange.Redis;

namespace MWLiteMiddleWare
{
    internal class DbHelper
    {
        private readonly ConnectionMultiplexer m_Connection;

        public DbHelper(string uri) { m_Connection = ConnectionMultiplexer.Connect(uri); }

        public WorkingConfig GetWorkLoad()
        {
            var db = m_Connection.GetDatabase();
            while (true)
            {
                var res = db.ListLeftPop("MWLiteWorkLoad");
                if (!res.HasValue)
                    Thread.Sleep(1000);
                else
                {
                    var config = JsonConvert.DeserializeObject<WorkingConfig>(res.ToString());
                    db.StringIncrement($"Working:{Hash(config.Config)}", config.Repetition, CommandFlags.FireAndForget);
                    return config;
                }
            }
        }

        public void PutResult(Configuration config, long[] result)
        {
            var db = m_Connection.GetDatabase();
            var trans = db.CreateTransaction();
            var keyBase = Hash(config);
            var cnt = 0L;
            for (var i = 0; i < result.Length; i++)
            {
                var val = result[i];
                cnt += val;
                if (val == 0)
                    continue;
                var key = $"{keyBase}:{i}";
                trans.StringIncrementAsync(key, val);
            }
            trans.StringDecrementAsync($"Working:{Hash(config)}", cnt);
            trans.Execute();
        }

        private static string Hash(Configuration config)
        {
            var sb = new StringBuilder();
            switch (config.Logic)
            {
                //case LogicMethod.ZeroLogic:
                //    sb.Append("ZL");
                //    break;
                case LogicMethod.Passive:
                    sb.Append("PL");
                    break;
                case LogicMethod.Single:
                    sb.Append("SL");
                    break;
                case LogicMethod.SingleExtended:
                    sb.Append("SLE");
                    break;
                case LogicMethod.Double:
                    sb.Append("DL");
                    break;
                case LogicMethod.DoubleExtended:
                    sb.Append("DLE");
                    break;
                case LogicMethod.Full:
                    sb.Append("FL");
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
            if (config.InitialPosition >= 0)
                sb.Append(
                          $"@[{(config.InitialPosition % config.Height) + 1}," +
                          $"{(config.InitialPosition / config.Height) + 1}]");
            sb.Append("-");
            if (!config.HeuristicEnabled)
                sb.Append("Pure");
            else if (config.DecisionTree.Count == 0)
                sb.Append("NH");
            else
                foreach (var method in config.DecisionTree)
                    switch (method)
                    {
                        case HeuristicMethod.MinMineProb:
                            sb.Append("P");
                            break;
                        case HeuristicMethod.MaxZeroProb:
                            sb.Append("Z");
                            break;
                        case HeuristicMethod.MaxZerosProb:
                            sb.Append("S");
                            break;
                        case HeuristicMethod.MaxZerosExp:
                            sb.Append("E");
                            break;
                        case HeuristicMethod.MaxQuantityExp:
                            sb.Append("Q");
                            break;
                        case HeuristicMethod.MinFrontierDist:
                            sb.Append("F");
                            break;
                        case HeuristicMethod.MaxUpperBound:
                            sb.Append("U");
                            break;
                        default:
                            throw new ArgumentOutOfRangeException();
                    }
            if (config.ExhaustEnabled)
                sb.Append($"-D{config.ExhaustCriterion}");
            sb.Append($"-{config.Width}-{config.Height}-");
            sb.Append($"T{config.TotalMines}");
            sb.Append("-NR");
            return sb.ToString();
        }

        public void PutException(string s)
        {
            var db = m_Connection.GetDatabase();
            db.ListRightPush("Exception", s, flags: CommandFlags.FireAndForget);
        }
    }
}
