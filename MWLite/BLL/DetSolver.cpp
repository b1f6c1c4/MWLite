#include "DetSolver.h"
#include "../stdafx.h"

IDetSolver::IDetSolver() { }

IDetSolver::~IDetSolver() {}

DetSolverManager::DetSolverManager(std::shared_ptr<IDetSolver> slv) : m_Solver(slv) { }

Block DetSolverManager::Decide(const State &state, const CancellationToken &cancel)
{
    BlockSet target(state.ClosedBlocks->FullSize());
    m_Solver->Decide(state, target, cancel);

    ASSERT((target - *state.ClosedBlocks).Count() == 0);

    if (target.Count() == 0)
        return m_Chooser.Choose(*state.ClosedBlocks);
    return m_Chooser.Choose(target);
}
