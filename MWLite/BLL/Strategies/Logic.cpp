#include "Logic.h"
#include "../../stdafx.h"
#include "StrategyMacros.h"

Strategies::LogicStrategy::LogicStrategy() : Mt(0), Bt(0) {}

void Strategies::LogicStrategy::Decide(const State &state, BlockSet &target, const CancellationToken &cancel)
{
    Make(state);

    Updates(state, cancel);

    Collect(state, target);
}

void Strategies::LogicStrategy::Make(const State &state)
{
    if (Mt.FullSize() != M.FullSize())
        Mt = M;
    else
        Mt += M;

    if (Mt.FullSize() != B.FullSize())
        Bt = B + !C - M;
    else
        Bt += B + !C - M;
}

void Strategies::LogicStrategy::Updates(const State &state, const CancellationToken &cancel)
{
    REPEAT
          {
              if (cancel.IsCancelled())
                  return;

              Update(state);
          }
          UNTIL(Mt.Count())
          UNTIL(Bt.Count())
          END_REPEAT
}

void Strategies::LogicStrategy::Collect(const State &state, BlockSet &target)
{
    target = Bt * C;
    if (target.Count() == 0)
        target = C - Mt;
}

void Strategies::Extended::Update(const State &state, BlockSet &Mt, BlockSet &Bt)
{
    if (!state.Config->UseTotalMines)
        return;

    for (auto b : B)
        if (f(b) + (Mt - U(b)).Count() == state.Config->TotalMines)
            Bt += C - U(b) - Mt;
}
