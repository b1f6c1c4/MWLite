#include "SolverBuilder.h"
#include "../../stdafx.h"
#include "NoLogic.h"
#include "SingleLogic.h"

std::mutex SolverBuilder::TheMtx;
std::unique_ptr<SolverBuilder> SolverBuilder::TheInstance;

SolverBuilder &SolverBuilder::Instance()
{
    if (TheInstance == nullptr)
    {
        std::lock_guard<std::mutex> lock(TheMtx);

        if (TheInstance == nullptr)
            TheInstance = std::make_unique<SolverBuilder>();
    }

    return *TheInstance;
}

std::shared_ptr<ISolver> SolverBuilder::Build(const std::string &str) const
{
    std::shared_ptr<IDetSolver> tmp;
    if (str == "zl")
        tmp = BuildZeroLogic();
    else if (str == "pl")
        tmp = BuildPassiveLogic();
    else if (str == "sl")
        tmp = BuildSingleLogic();
    else if (str == "sle")
        tmp = BuildSingleLogicExtended();
    else
        return nullptr;

    return std::make_shared<DetSolverManager>(tmp);
}

std::shared_ptr<IDetSolver> SolverBuilder::BuildZeroLogic() const
{
    return std::make_shared<Strategies::ZeroLogic>();
}

std::shared_ptr<IDetSolver> SolverBuilder::BuildPassiveLogic() const
{
    return std::make_shared<Strategies::PassiveLogic>();
}

std::shared_ptr<IDetSolver> SolverBuilder::BuildSingleLogic() const
{
    return std::make_shared<Strategies::SingleLogic>();
}

std::shared_ptr<IDetSolver> SolverBuilder::BuildSingleLogicExtended() const
{
    return std::make_shared<Strategies::SingleLogicExtended>();
}
