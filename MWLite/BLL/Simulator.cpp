#include "Simulator.h"

Simulator::Simulator(const Game &game, std::shared_ptr<ISolver> slv) : m_Game(game), m_Solver(slv), ClosedBlocks(std::make_shared<BlockSet>(game.AllMines.FullSize())), OpenMines(std::make_shared<BlockSet>(game.AllMines.FullSize())), OpenNoMines(std::make_shared<BlockSet>(game.AllMines.FullSize())), m_ToOpen(game.AllMines.FullSize())
{
    // all blocks are closed
    *ClosedBlocks = !*ClosedBlocks;

    // make deleted neighborhoods
    BlockSet blkR(m_Game.AllMines.FullSize());
    for (Block id = 0; id < m_Game.AllMines.FullSize(); id++)
    {
        blkR.Clear();
        auto i = GetX(id);
        auto j = GetY(id);
        for (auto di = -1; di <= 1; ++di)
            if (i + di >= 0 && i + di < m_Game.Config->Width)
                for (auto dj = -1; dj <= 1; ++dj)
                    if (j + dj >= 0 && j + dj < m_Game.Config->Height)
                        if (di != 0 || dj != 0)
                            blkR += GetIndex(i + di, j + dj);
        m_DeletedNeighorhood.emplace_back(blkR);
    }

    // count ToOpen
    ResetToOpen();
}

Simulator::~Simulator() { }

int Simulator::Solve(const CancellationToken &cancel, std::function<void(int)> adjuster)
{
    auto flag = true;

    State state;
    state.Config = m_Game.Config;
    state.ClosedBlocks = ClosedBlocks;
    state.OpenMines = OpenMines;
    state.OpenNoMines = OpenNoMines;
    state.Neighborhood = [this](Block blk)
        {
            return &m_DeletedNeighorhood[blk];
        };
    state.NeighborCount = [this](Block blk)
        {
            return (m_DeletedNeighorhood[blk] * m_Game.AllMines).Count();
        };

    while (m_ToOpen > 0)
    {
        if (cancel.IsCancelled())
            return -1;

        auto blk = m_Solver->Decide(state, cancel);

        if (flag)
        {
            adjuster(blk);
            ResetToOpen();
            flag = false;
        }

        ASSERT((*ClosedBlocks)[blk]);
        *ClosedBlocks -= blk;
        if (m_Game.AllMines[blk])
            *OpenMines += blk;
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
    return x * m_Game.Config->Height + y;
}

int Simulator::GetX(int id) const
{
    return id / m_Game.Config->Height;
}

int Simulator::GetY(int id) const
{
    return id % m_Game.Config->Height;
}

void Simulator::ResetToOpen()
{
    m_ToOpen = m_Game.Config->Width * m_Game.Config->Height;
    if (m_Game.Config->UseTotalMines)
        m_ToOpen -= m_Game.Config->TotalMines;
    else
        m_ToOpen -= m_Game.AllMines.Count();
}
