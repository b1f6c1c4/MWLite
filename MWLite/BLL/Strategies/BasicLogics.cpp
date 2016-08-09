#include "../../stdafx.h"
#include "BasicLogics.h"

#define _B (*state.B)
#define _C (*state.C)
#define _M (*state.M)
#define _U (state.U)
#define _f (state.f)

void Strategies::ZeroLogic::Decide(const State &state, BlockSet &target, const CancellationToken &cancel)
{
    // does nothing
}

void Strategies::PassiveLogic::Decide(const State &state, BlockSet &target, const CancellationToken &cancel)
{
    for (auto b : _B)
        if (_f(b) == 0)
            target += _U(b) * _C;
}

void Strategies::SingleLogic::Decide(const State& state, BlockSet& target, const CancellationToken& cancel)
{
    auto Mt = _M;
    auto Bt = _B + !_C - _M;

    auto oldMt = Mt.Count() + 1, oldBt = Bt.Count() + 1;
    while (oldMt != Mt.Count() || oldBt != Bt.Count())
    {
        oldMt = Mt.Count(), oldBt = Bt.Count();
        for (auto b : _B)
        {
            auto tmp = _U(b) - Bt;
            if (_f(b) == tmp.Count())
                Mt += tmp;
            tmp = _U(b) * Mt;
            if (_f(b) == tmp.Count())
                Bt += tmp;
        }
    }

    target = Bt * _C;
    if (target.Count() == 0)
        target = _C - Mt;
}

void Strategies::SingleLogicExtended::Decide(const State& state, BlockSet& target, const CancellationToken& cancel)
{
    auto Mt = _M;
    auto Bt = _B + !_C - _M;

    auto oldMt = Mt.Count() + 1, oldBt = Bt.Count() + 1;
    while (oldMt != Mt.Count() || oldBt != Bt.Count())
    {
        oldMt = Mt.Count(), oldBt = Bt.Count();
        for (auto b : _B)
        {
            auto tmp = _U(b) - Bt;
            if (_f(b) == tmp.Count())
                Mt += tmp;
            tmp = _U(b) * Mt;
            if (_f(b) == tmp.Count())
                Bt += tmp;
        }

        if (!state.Config->UseTotalMines)
            continue;

        for (auto b : _B)
            if (_f(b) + (Mt - _U(b)).Count() == state.Config->TotalMines)
                Bt += _C - _U(b) - Mt;
    }

    target = Bt * _C;
    if (target.Count() == 0)
        target = _C - Mt;
}
