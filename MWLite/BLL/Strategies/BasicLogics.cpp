#include "../../stdafx.h"
#include "BasicLogics.h"

#define B (*state.OpenNoMines)
#define C (*state.ClosedBlocks)
#define M (*state.OpenMines)
#define U (state.Neighborhood)
#define f (state.NeighborCount)

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

void Strategies::SingleLogic::Decide(const State& state, BlockSet& target, const CancellationToken& cancel)
{
    auto Mt = M;
    auto Bt = B + !C - M;

    auto oldMt = Mt.Count() + 1, oldBt = Bt.Count() + 1;
    while (oldMt != Mt.Count() || oldBt != Bt.Count())
    {
        oldMt = Mt.Count(), oldBt = Bt.Count();
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

    target = Bt * C;
    if (target.Count() == 0)
        target = C - Mt;
}

void Strategies::SingleLogicExtended::Decide(const State& state, BlockSet& target, const CancellationToken& cancel)
{
    auto Mt = M;
    auto Bt = B + !C - M;

    auto oldMt = Mt.Count() + 1, oldBt = Bt.Count() + 1;
    while (oldMt != Mt.Count() || oldBt != Bt.Count())
    {
        oldMt = Mt.Count(), oldBt = Bt.Count();
        for (auto b : B)
        {
            auto tmp = U(b) - Bt;
            if (f(b) == tmp.Count())
                Mt += tmp;
            tmp = U(b) * Mt;
            if (f(b) == tmp.Count())
                Bt += tmp;
        }

        if (!state.Config->UseTotalMines)
            continue;

        for (auto b : B)
            if (f(b) + (Mt - U(b)).Count() == state.Config->TotalMines)
                Bt += C - U(b) - Mt;
    }

    target = Bt * C;
    if (target.Count() == 0)
        target = C - Mt;
}
