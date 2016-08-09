#include "../../stdafx.h"
#include "NoLogic.h"
#include "StrategyMacros.h"

void Strategies::ZeroLogic::Decide(const State &state, BlockSet &target, const CancellationToken &cancel)
{
    // do nothing
}

void Strategies::PassiveLogic::Decide(const State &state, BlockSet &target, const CancellationToken &cancel)
{
    for (auto b : B)
        if (f(b) == 0)
            target += U(b) * C;
}
