#include "Worker.h"
#include "../../MineSweeperProb/MineSweeperSolver/BinomialHelper.h"

Worker::Worker() { }

void Worker::Prepare()
{
    CacheBinomials(Config.Width * Config.Height, Config.TotalMines);
}

size_t Worker::ProcessOne()
{
    auto imme = false;
    GameMgr mgr(Config.Width, Config.Height, Config.TotalMines, Config, !imme);
    mgr.Automatic();
    if (imme)
        return mgr.GetSucceed() ? 0 : 1;
    return mgr.GetWrongGuesses();
}
