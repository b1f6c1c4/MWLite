#pragma once
#include "../stdafx.h"
#include "../Entities/Configuration.h"
#include "../Entities/BlockSet.h"
#include <functional>

struct State sealed
{
    std::weak_ptr<BaseConfiguration> Config;

    // closed blocks
    std::weak_ptr<BlockSet> C;

    // open blocks with mine
    std::weak_ptr<BlockSet> M;

    // open blocks without mine
    std::weak_ptr<BlockSet> B;

    // get the deleted neighborhood
    std::function<const BlockSet &(Block)> U;

    // get total mines in the deleted neighborhood
    std::function<size_t(Block)> f;
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
