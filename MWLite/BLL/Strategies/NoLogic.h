#pragma once
#include "../../stdafx.h"
#include "../DetSolver.h"

namespace Strategies
{
    class ZeroLogic : public IDetSolver
    {
    public:
        void Decide(const State &state, BlockSet &target, const CancellationToken &cancel) override;
    };

    class PassiveLogic : public IDetSolver
    {
    public:
        void Decide(const State &state, BlockSet &target, const CancellationToken &cancel) override;
    };
}
