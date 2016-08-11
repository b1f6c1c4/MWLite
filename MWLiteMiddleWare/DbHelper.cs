using System;
using System.Text;
using Newtonsoft.Json;
using ServiceStack.Redis;

namespace MWLiteMiddleWare
{
    internal class DbHelper
    {
        private readonly string m_Uri;

        public DbHelper(string uri) { m_Uri = uri; }

        public WorkingConfig GetWorkLoad()
        {
            using (var client = new RedisClient(m_Uri))
            {
                var str = client.BlockingPopItemFromList("MWLiteWorkLoad", null);
                return JsonConvert.DeserializeObject<WorkingConfig>(str);
            }
        }

        public void PutResult(Configuration config, LogicLevel level, long[] result)
        {
            using (var client = new RedisClient(m_Uri))
            using (var trans = client.CreateTransaction())
            {
                var keyBase = Hash(config, level);
                for (var i = 0; i < result.Length; i++)
                {
                    var val = result[i];
                    if (val == 0)
                        continue;
                    var key = $"{keyBase}:{i}";
                    trans.QueueCommand(c => c.IncrementValueBy(key, val));
                }
                trans.Commit();
            }
        }

        private static string Hash(Configuration config, LogicLevel level)
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
            sb.Append($"-{config.Width}-{config.Height}-");
            sb.Append(config.UseTotalMines ? $"T{config.TotalMines}" : $"P{config.Probability:R}");
            if (config.NotRigorous)
                sb.Append("-NR");
            return sb.ToString();
        }
    }
}
