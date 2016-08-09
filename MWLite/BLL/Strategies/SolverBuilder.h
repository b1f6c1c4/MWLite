#pragma once
#include "../../stdafx.h"
#include "../DetSolver.h"
#include <mutex>

class SolverBuilder sealed
{
public:
    static SolverBuilder &Instance();

    std::shared_ptr<ISolver> Build(LogicLevel level) const;

private:
    static std::mutex TheMtx;
    static std::unique_ptr<SolverBuilder> TheInstance;
};
