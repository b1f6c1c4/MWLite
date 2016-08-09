#include "SolverBuilder.h"
#include "../../stdafx.h"
#include "NoLogic.h"
#include "SingleLogic.h"
#include "DoubleLogic.h"

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

std::shared_ptr<ISolver> SolverBuilder::Build(LogicLevel level) const
{
    std::shared_ptr<IDetSolver> tmp;
    switch (level)
    {
    case LogicLevel::ZeroLogic:
        tmp = std::make_unique<Strategies::ZeroLogic>();
        break;
    case LogicLevel::PassiveLogic:
        tmp = std::make_unique<Strategies::PassiveLogic>();
        break;
    case LogicLevel::SingleLogic:
        tmp = std::make_unique<Strategies::SingleLogic>();
        break;
    case LogicLevel::SingleLogicExtended:
        tmp = std::make_unique<Strategies::SingleLogicExtended>();
        break;
    case LogicLevel::DoubleLogic:
        tmp = std::make_unique<Strategies::DoubleLogic>();
        break;
    case LogicLevel::DoubleLogicExtended:
        tmp = std::make_unique<Strategies::DoubleLogicExtended>();
        break;
    case LogicLevel::FullLogic:
        // tmp = std::make_unique<Strategies::FullLogic>();
        // break;
    default:
        return nullptr;
    }

    return std::make_shared<DetSolverManager>(tmp);
}
