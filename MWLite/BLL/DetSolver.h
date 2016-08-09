#pragma once
#include "ISolver.h"
#include "RandomChooser.h"

class DetSolver
{
public:
    DetSolver();

    void SetBase(std::shared_ptr<DetSolver> base);

    virtual void Decide(const State& state, BlockSet &target, const CancellationToken& cancel);

protected:
    virtual ~DetSolver();

private:
    std::shared_ptr<DetSolver> m_Base;
};

class DetSolverManager sealed : public ISolver
{
public:
    explicit DetSolverManager(std::shared_ptr<DetSolver> slv);

    NO_COPY(DetSolverManager);
    NO_MOVE(DetSolverManager);

    Block Decide(const State& state, const CancellationToken& cancel) override;

private:
    RandomBlockChooser m_Chooser;

    std::shared_ptr<DetSolver> m_Solver;
};
