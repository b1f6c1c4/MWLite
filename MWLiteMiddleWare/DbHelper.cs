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

        public void PutResult(Configuration config, long[] result)
        {
            var db = m_Connection.GetDatabase();
            var trans = db.CreateTransaction();
            var keyBase = Hash(config);
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
            sb.Append($"-{config.Width}-{config.Height}-");
            sb.Append($"T{config.TotalMines}");
            sb.Append("-NR");
            return sb.ToString();
        }
    }
}
