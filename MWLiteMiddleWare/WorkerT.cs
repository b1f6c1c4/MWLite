namespace MWLiteMiddleWare
{
    internal class WorkerT : WorkerBase
    {
        public WorkerT(DbHelper db) : base(db) { }

        protected override void Put(WorkingConfig work, long[] result) => Db.PutT(work.Config, result);

        protected override WorkingConfig GetWorkLoad()
        {
            var work = Db.GetWorkLoadT();
            return work;
        }

        protected override void Make(WorkingConfig cfg)
        {
            lock (Lock)
                TheWorker = DllWrapper.MakeWorker(cfg.Config, cfg.Repetition, true);
        }
    }
}
