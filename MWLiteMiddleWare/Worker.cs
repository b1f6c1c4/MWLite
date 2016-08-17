namespace MWLiteMiddleWare
{
    internal class Worker : WorkerBase
    {
        public Worker(DbHelper db) : base(db) { }

        protected override void Put(WorkingConfig work, long[] result) => Db.PutResult(work.Config, result);

        protected override WorkingConfig GetWorkLoad()
        {
            var work = Db.GetWorkLoad();
            return work;
        }

        protected override void Make(WorkingConfig cfg)
        {
            lock (Lock)
                TheWorker = DllWrapper.MakeWorker(cfg.Config, cfg.Repetition, false);
        }
    }
}
