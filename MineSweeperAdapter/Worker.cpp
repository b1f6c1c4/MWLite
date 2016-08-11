#include "Worker.h"
#include "../../MineSweeperProb/MineSweeperSolver/BinomialHelper.h"
#include "../../MineSweeperProb/MineSweeperSolver/Strategies.h"

Worker::Worker() { }

void Worker::Prepare()
{
    if (!m_Config->UseTotalMines)
        throw std::runtime_error("not implemented");

    CacheBinomials(m_Config->TotalBlocks(), m_Config->TotalMines);

    m_Strategy.InitialPositionSpecified = false;
    m_Strategy.PruningEnabled = false;
    m_Strategy.ExhaustEnabled = false;

    m_Strategy.HeuristicEnabled = true; // NH
    switch (m_Logic)
    {
    case LogicLevel::ZeroLogic:
    case LogicLevel::PassiveLogic:
        m_Strategy.Logic = LogicMethod::None; // TODO
        break;
    case LogicLevel::SingleLogic:
    case LogicLevel::SingleLogicExtended:
        m_Strategy.Logic = LogicMethod::Single; // TODO
        break;
    case LogicLevel::DoubleLogic:
    case LogicLevel::DoubleLogicExtended:
        m_Strategy.Logic = LogicMethod::Double; // TODO
        break;
    case LogicLevel::FullLogic:
        m_Strategy.Logic = LogicMethod::Full;
        break;
    default:
        throw std::runtime_error("not implemented");
    }
}

size_t Worker::ProcessOne()
{
    auto imme = false;
    GameMgr mgr(m_Config->Width, m_Config->Height, m_Config->TotalMines, imme);
    mgr.BasicStrategy = m_Strategy;
    mgr.Automatic();
    if (imme)
        return mgr.GetSucceed() ? 0 : 1;
    return mgr.GetWrongGuesses();
}
