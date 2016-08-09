#pragma once
#include "../../stdafx.h"
#include "../DetSolver.h"
#include <mutex>

class SolverBuilder sealed
{
public:
    static SolverBuilder &Instance();

    std::shared_ptr<ISolver> Build(const std::string &str) const;

private:
    static std::mutex TheMtx;
    static std::unique_ptr<SolverBuilder> TheInstance;

    std::shared_ptr<DetSolver> BuildZeroLogic() const;
    std::shared_ptr<DetSolver> BuildPassiveLogic() const;
    std::shared_ptr<DetSolver> BuildSingleLogic() const;
    std::shared_ptr<DetSolver> BuildSingleLogicExtended() const;
};
