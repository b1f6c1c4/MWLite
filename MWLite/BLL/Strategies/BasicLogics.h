#pragma once
#include "../../stdafx.h"
#include "../DetSolver.h"

namespace Strategies
{
    class ZeroLogic : public DetSolver
    {
    public:
        void Decide(const State &state, BlockSet &target, const CancellationToken &cancel) override;
    };

    class PassiveLogic : public DetSolver
    {
    public:
        void Decide(const State &state, BlockSet &target, const CancellationToken &cancel) override;
    };

    class SingleLogic : public DetSolver
    {
    public:
        void Decide(const State &state, BlockSet &target, const CancellationToken &cancel) override;
    };

    class SingleLogicExtended : public DetSolver
    {
    public:
        void Decide(const State &state, BlockSet &target, const CancellationToken &cancel) override;
    };
}
