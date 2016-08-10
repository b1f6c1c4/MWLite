#include "Simulator.h"

Simulator::Simulator(std::shared_ptr<Configuration> config, std::shared_ptr<IGenerator> gameGen, std::shared_ptr<SolverBuilder> solverGen, bool imme) : m_Config(config), m_GameGen(gameGen), m_SolverGen(solverGen), m_Imme(imme), m_Game(std::make_shared<Game>()), ClosedBlocks(std::make_shared<BlockSet>(config->Mines())), OpenMines(std::make_shared<BlockSet>(config->Mines())), OpenNoMines(std::make_shared<BlockSet>(config->Mines())), m_ToOpen(0)
{
    m_Game->Config = m_Config;

    // all blocks are closed
    *ClosedBlocks = !*ClosedBlocks;

    // make deleted neighborhoods
    SparseBlockSet blkR(m_Config->Mines());
    for (Block id = 0; id < m_Config->Mines(); id++)
    {
        blkR.Clear();
        auto i = GetX(id);
        auto j = GetY(id);
        for (auto di = -1; di <= 1; ++di)
            if (i + di >= 0 && i + di < m_Config->Width)
                for (auto dj = -1; dj <= 1; ++dj)
                    if (j + dj >= 0 && j + dj < m_Config->Height)
                        if (di != 0 || dj != 0)
                            blkR += GetIndex(i + di, j + dj);
        m_DeletedNeighborhood.push_back(blkR);
    }
}

Simulator::~Simulator() { }

int Simulator::Simulate(const CancellationToken &cancel)
{
    m_GameGen->GenerateGame(*m_Game);
    m_Solver = m_SolverGen->Build();
    ResetToOpen();
    // don't calculate now; use less
    // ResetNeighborCount();

    auto flag = true;

    State state;
    state.Config = m_Game->Config;
    state.ClosedBlocks = ClosedBlocks;
    state.OpenMines = OpenMines;
    state.OpenNoMines = OpenNoMines;
    state.Neighborhood = [this](Block blk)
        {
            return &m_DeletedNeighborhood[blk];
        };
    state.NeighborCount = [this](Block blk)
        {
            return m_NeighborCount[blk];
        };

    while (m_ToOpen > 0)
    {
        if (cancel.IsCancelled())
            return -1;

        auto blk = m_Solver->Decide(state, cancel);

        if (flag)
        {
            m_GameGen->AdjustGame(*m_Game, blk);
            ResetToOpen();
            ResetNeighborCount();
            flag = false;
        }

        ASSERT((*ClosedBlocks)[blk]);
        *ClosedBlocks -= blk;
        if (m_Game->AllMines[blk])
        {
            *OpenMines += blk;
            if (m_Imme)
                break;
        }
        else
        {
            *OpenNoMines += blk;
            m_ToOpen--;
        }
    }
    return OpenMines->Count();
}

int Simulator::GetIndex(int x, int y) const
{
    return x * m_Config->Height + y;
}

int Simulator::GetX(int id) const
{
    return id / m_Config->Height;
}

int Simulator::GetY(int id) const
{
    return id % m_Config->Height;
}

void Simulator::ResetToOpen()
{
    m_ToOpen = m_Config->Mines();
    if (m_Config->UseTotalMines)
        m_ToOpen -= m_Config->TotalMines;
    else
        m_ToOpen -= m_Game->AllMines.Count();
}

void Simulator::ResetNeighborCount()
{
    m_NeighborCount.reserve(m_Config->Mines());
    for (Block id = 0; id < m_Config->Mines(); id++)
        m_NeighborCount.push_back((m_DeletedNeighborhood[id] * m_Game->AllMines).Count());
}
