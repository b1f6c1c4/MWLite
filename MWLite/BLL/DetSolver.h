#pragma once
#include "ISolver.h"
#include "RandomChooser.h"

class IDetSolver
{
public:
    virtual void Decide(const State& state, BlockSet &target, const CancellationToken& cancel) = 0;

protected:
    IDetSolver();
    virtual ~IDetSolver();
};

class DetSolverManager sealed : public ISolver
{
public:
    explicit DetSolverManager(std::shared_ptr<IDetSolver> slv);

    NO_COPY(DetSolverManager);
    NO_MOVE(DetSolverManager);

    Block Decide(const State& state, const CancellationToken& cancel) override;

private:
    RandomBlockChooser m_Chooser;

    std::shared_ptr<IDetSolver> m_Solver;
};
