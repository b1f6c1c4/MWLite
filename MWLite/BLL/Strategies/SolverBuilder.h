#pragma once
#include "../../stdafx.h"
#include "../DetSolver.h"

class SolverBuilder sealed
{
public:
    // ReSharper disable once CppNonExplicitConvertingConstructor
    SolverBuilder(LogicLevel level);

    std::shared_ptr<ISolver> Build() const;

private:
    LogicLevel m_Logic;
};
