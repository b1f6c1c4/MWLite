#pragma once
#include "stdafx.h"
#include "../MWLiteFundamental/BLL/BaseWorker.h"
#include "../../MineSweeperProb/MineSweeperSolver/GameMgr.h"

class Worker : public BaseWorker
{
public:
    Worker();

    NO_COPY(Worker);
    NO_MOVE(Worker);

protected:
    void Prepare() override;
    size_t ProcessOne() override;

private:
    Strategy m_Strategy;
};
