#pragma once
#include "../../stdafx.h"
#include "Logic.h"

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
    };

    class DoubleLogicExtended : public LogicStrategy
    {
    protected:
        void Update(const State &state) override;
    };
}
