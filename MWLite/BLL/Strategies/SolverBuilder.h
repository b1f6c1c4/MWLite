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

    std::shared_ptr<IDetSolver> BuildZeroLogic() const;
    std::shared_ptr<IDetSolver> BuildPassiveLogic() const;
    std::shared_ptr<IDetSolver> BuildSingleLogic() const;
    std::shared_ptr<IDetSolver> BuildSingleLogicExtended() const;
};
