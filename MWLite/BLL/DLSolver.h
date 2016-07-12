#pragma once
#include "../stdafx.h"
#include "SLSolver.h"

class DLSolver : public SLSolver
{
public:
    explicit DLSolver(const Game &game);
    ~DLSolver();

protected:
    void Simplify() override;

    bool SimpleOverlapAll();

    bool SimpleOverlap(int r1, int r2);

private:
    bool *m_Pairs_Temp;
    size_t m_Pairs_Temp_Size;

    std::vector<Container> m_OverlapA_Temp, m_OverlapB_Temp, m_OverlapC_Temp;
};
