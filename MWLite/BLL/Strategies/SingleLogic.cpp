#include "SingleLogic.h"
#include "../../stdafx.h"
#include "StrategyMacros.h"

void Strategies::Single::Update(const State &state, BlockSet &Mt, BlockSet &Bt)
{
    for (auto b : B)
    {
        auto tmp = U(b) - Bt;
        if (f(b) == tmp.Count())
            Mt += tmp;
        tmp = U(b) * Mt;
        if (f(b) == tmp.Count())
            Bt += tmp;
    }
}

void Strategies::SingleLogic::Update(const State &state)
{
    USE_LOGIC(Single);
}

void Strategies::SingleLogicExtended::Update(const State &state)
{
    USE_LOGIC(Single);
    USE_LOGIC(Extended);
}
