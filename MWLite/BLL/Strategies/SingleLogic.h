#pragma once
#include "../../stdafx.h"
#include "Logic.h"

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
    };

    class SingleLogicExtended : public LogicStrategy
    {
    protected:
        void Update(const State &state) override;
    };
}
