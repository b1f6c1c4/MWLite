#pragma once
#include "../../stdafx.h"
#include "Logic.h"
#include "StrategyMacros.h"
#include "SingleLogic.h"

namespace Strategies
{
    class Double : public ILogic
    {
    public:
        void Update(const State &state, BlockSet &Mt, BlockSet &Bt) override;
    };

    class DoubleLogic : public LogicStrategy
    {
    protected:
        void Update(const State &state) override;
    private:
        USE_LOGIC_T(Single);
        USE_LOGIC_T(Double);
    };

    class DoubleLogicExtended : public LogicStrategy
    {
    protected:
        void Update(const State &state) override;
    private:
        USE_LOGIC_T(Single);
        USE_LOGIC_T(Double);
        USE_LOGIC_T(Extended);
    };
}
