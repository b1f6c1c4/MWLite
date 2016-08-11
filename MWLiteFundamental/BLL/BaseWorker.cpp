#include "BaseWorker.h"

BaseWorker::BaseWorker() : Logic(LogicLevel::ZeroLogic), Repetition(0) { }

BaseWorker::~BaseWorker() { }

void BaseWorker::Process()
{
    Prepare();

    while (Repetition > 0)
    {
        if (m_Cancel.IsCancelled())
            break;

        auto res = ProcessOne();

        if (m_Cancel.IsCancelled())
            break;

        if (res >= Result.size())
            Result.resize(res + 1, 0);
        Result[res]++;

        Repetition--;
    }
}

void BaseWorker::Cancel()
{
    m_Cancel.Cancel();
}

void BaseWorker::Prepare() { }
