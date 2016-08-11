#include "Worker.h"
#include "../../MineSweeperProb/MineSweeperSolver/BinomialHelper.h"
#include "../../MineSweeperProb/MineSweeperSolver/Strategies.h"

Worker::Worker() { }

void Worker::Prepare()
{
    if (!Config->UseTotalMines)
        throw std::runtime_error("not implemented");

    CacheBinomials(Config->TotalBlocks(), Config->TotalMines);

    m_Strategy.InitialPositionSpecified = false;
    m_Strategy.PruningEnabled = false;
    m_Strategy.ExhaustEnabled = false;

    m_Strategy.HeuristicEnabled = true; // NH
    switch (Logic)
    {
    case LogicLevel::PassiveLogic:
        m_Strategy.Logic = LogicMethod::Passive;
        break;
    case LogicLevel::SingleLogic:
        m_Strategy.Logic = LogicMethod::Single;
        break;
    case LogicLevel::SingleLogicExtended:
        m_Strategy.Logic = LogicMethod::SingleExtended;
        break;
    case LogicLevel::DoubleLogic:
        m_Strategy.Logic = LogicMethod::Double;
        break;
    case LogicLevel::DoubleLogicExtended:
        m_Strategy.Logic = LogicMethod::DoubleExtended;
        break;
    case LogicLevel::FullLogic:
        m_Strategy.Logic = LogicMethod::Full;
        break;
    case LogicLevel::ZeroLogic:
    default:
        throw std::runtime_error("not implemented");
    }
}

size_t Worker::ProcessOne()
{
    auto imme = false;
    GameMgr mgr(Config->Width, Config->Height, Config->TotalMines, m_Strategy, !imme);
    mgr.Automatic();
    if (imme)
        return mgr.GetSucceed() ? 0 : 1;
    return mgr.GetWrongGuesses();
}
