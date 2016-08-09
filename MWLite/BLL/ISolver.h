#pragma once
#include "../stdafx.h"
#include "../Entities/Configuration.h"
#include "../Entities/BlockSet.h"
#include <functional>

struct State sealed
{
    std::shared_ptr<Configuration> Config;

    std::shared_ptr<BlockSet> ClosedBlocks;

    std::shared_ptr<BlockSet> OpenMines;

    std::shared_ptr<BlockSet> OpenNoMines;

    std::function<const BlockSet *(Block)> Neighborhood;

    std::function<size_t(Block)> NeighborCount;
};

class ISolver
{
public:
    virtual ~ISolver() { }

    // return < 0 means giving up
    virtual Block Decide(const State &state, const CancellationToken &cancel) = 0;

protected:
    ISolver() { }
};
