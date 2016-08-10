#pragma once
#include "../../stdafx.h"
#include "Logic.h"
#include "StrategyMacros.h"

namespace Strategies
{
    class Single : public ILogic
    {
    public:
        void Update(const State &state, BlockSet &Mt, BlockSet &Bt) override;
    };

    class SingleLogic : public LogicStrategy
    {
    protected:
        void Update(const State &state) override;
    private:
        USE_LOGIC_T(Single);
    };

    class SingleLogicExtended : public LogicStrategy
    {
    protected:
        void Update(const State &state) override;
    private:
        USE_LOGIC_T(Single);
        USE_LOGIC_T(Extended);
    };
}
