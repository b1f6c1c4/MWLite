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
                    return JsonConvert.DeserializeObject<WorkingConfig>(res.ToString());
            }
        }

        public void PutResult(Configuration config, LogicLevel level, long[] result)
        {
            var db = m_Connection.GetDatabase();
            var trans = db.CreateTransaction();
            var keyBase = Hash(config, level);
            for (var i = 0; i < result.Length; i++)
            {
                var val = result[i];
                if (val == 0)
                    continue;
                var key = $"{keyBase}:{i}";
                trans.StringIncrementAsync(key, val);
            }
            trans.Execute();
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
