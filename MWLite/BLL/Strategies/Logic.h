#pragma once
#include "../../stdafx.h"
#include "../DetSolver.h"

namespace Strategies
{
    class ILogic
    {
    public:
        virtual ~ILogic() {}

        virtual void Update(const State &state, BlockSet &Mt, BlockSet &Bt) = 0;
    };

    class LogicStrategy : public IDetSolver
    {
    public:

        void Decide(const State &state, BlockSet &target, const CancellationToken &cancel) override;

    protected:
        BlockSet Mt;
        BlockSet Bt;

        LogicStrategy();

        void Make(const State &state);
        void Updates(const State &state, const CancellationToken &cancel);
        virtual void Collect(const State &state, BlockSet &target);
        virtual void Update(const State &state) = 0;
    };

    class Extended : public ILogic
    {
    public:
        void Update(const State &state, BlockSet &Mt, BlockSet &Bt) override;
    };
}
