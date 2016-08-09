#pragma once
#include "../stdafx.h"
#include "../Entities/Configuration.h"
#include "../Entities/BlockSet.h"
#include <functional>

struct State sealed
{
    std::shared_ptr<Configuration> Config;

    // closed blocks
    std::shared_ptr<BlockSet> C;

    // open blocks with mine
    std::shared_ptr<BlockSet> M;

    // open blocks without mine
    std::shared_ptr<BlockSet> B;

    // total mines in the deleted neighborhood
    std::function<size_t(Block)> f;
};

class ISolver
{
public:
    virtual ~ISolver() { }

    // return < 0 means giving up
    virtual Block Decide(const State &state) = 0;

protected:
    ISolver() { }
};
