#include "Simulator.h"

Simulator::Simulator(std::shared_ptr<Configuration> config, std::shared_ptr<IGenerator> gameGen, std::shared_ptr<SolverBuilder> solverGen, bool imme) : m_Config(config), m_GameGen(gameGen), m_SolverGen(solverGen), m_Imme(imme), m_Game(std::make_shared<Game>()), ClosedBlocks(std::make_shared<BlockSet>(config->TotalBlocks())), OpenMines(std::make_shared<BlockSet>(config->TotalBlocks())), OpenNoMines(std::make_shared<BlockSet>(config->TotalBlocks())), m_ToOpen(0)
{
    m_Game->Config = m_Config;
    m_NeighborCount.resize(m_Config->TotalBlocks(), static_cast<size_t>(-1));

    // make deleted neighborhoods
    SparseBlockSet blkR(m_Config->TotalBlocks());
    for (Block id = 0; id < m_Config->TotalBlocks(); id++)
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

void Simulator::ResetNeighborCount()
{
    std::fill(m_NeighborCount.begin(), m_NeighborCount.end(), static_cast<size_t>(-1));
}

int Simulator::Simulate(const CancellationToken &cancel)
{
    m_GameGen->GenerateGame(*m_Game);
    m_Solver = m_SolverGen->Build();
    ResetToOpen();
    ResetNeighborCount();

    ClosedBlocks->Clear();
    *ClosedBlocks = !*ClosedBlocks;
    OpenMines->Clear();
    OpenNoMines->Clear();

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
            return GetNeighborCount(blk);
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
    m_ToOpen = m_Config->TotalBlocks();
    if (m_Config->UseTotalMines)
        m_ToOpen -= m_Config->TotalMines;
    else
        m_ToOpen -= m_Game->AllMines.Count();
}

size_t Simulator::GetNeighborCount(Block id)
{
    return m_NeighborCount[id] = (m_DeletedNeighborhood[id] * m_Game->AllMines).Count();
}
