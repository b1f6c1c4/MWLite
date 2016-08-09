#include "DetSolver.h"
#include "../stdafx.h"

DetSolver::DetSolver() : m_Base(nullptr) {}

void DetSolver::SetBase(std::shared_ptr<DetSolver> base)
{
    m_Base = base;
}

void DetSolver::Decide(const State &state, BlockSet &target, const CancellationToken &cancel)
{
    if (m_Base != nullptr)
        m_Base->Decide(state, target, cancel);
}

DetSolver::~DetSolver() {}

DetSolverManager::DetSolverManager(std::shared_ptr<DetSolver> slv) : m_Solver(slv) { }

Block DetSolverManager::Decide(const State &state, const CancellationToken &cancel)
{
    BlockSet target(state.C->FullSize());
    m_Solver->Decide(state, target, cancel);
    if (target.Count() == 0)
        return m_Chooser.Choose(*state.C);
    return m_Chooser.Choose(target);
}
