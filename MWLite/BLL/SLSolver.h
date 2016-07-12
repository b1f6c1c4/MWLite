#pragma once
#include "../stdafx.h"
#include "BasicSolver.h"

class SLSolver : public BasicSolver
{
public:
    explicit SLSolver(const Game &game);
    ~SLSolver();

protected:

    void Simplify() override;

    bool ReduceRestrainBlank(int row);
    bool ReduceRestrainMine(int row);
    void ReduceRestrains();

private:
    std::vector<size_t> m_ReduceCount_Temp;
};
