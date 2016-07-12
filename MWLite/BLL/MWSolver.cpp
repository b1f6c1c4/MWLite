#include "MWSolver.h"

MWSolver::MWSolver(const Game &game) : m_Game(game) , m_ToOpen(game.Width * game.Height), m_WrongGuesses(0)
{
    m_IsOpen.resize(game.Width * game.Height, false);

    m_Random.seed(std::random_device()());

    if (game.TotalMines >= 0)
    {
        m_ToOpen -= game.TotalMines;
        return;
    }

    for (auto i = 0; i < game.Width * game.Height; i++)
        if (game.IsMine[i])
            m_ToOpen--;
}

MWSolver::~MWSolver() { }

int MWSolver::Solve()
{
    while (m_ToOpen > 0)
    {
        auto blk = NextBlock();
        if (blk < 0)
            blk = RandomNonTrivial();

        OpenBlock(blk);
    }
    return m_WrongGuesses;
}

bool MWSolver::IsOpen(int id) const
{
    return m_IsOpen[id];
}

int MWSolver::GetIndex(int x, int y) const
{
    return x * m_Game.Height + y;
}

int MWSolver::GetX(int id) const
{
    return id / m_Game.Height;
}

int MWSolver::GetY(int id) const
{
    return id % m_Game.Height;
}

BlockSet MWSolver::GetBlockR(int id) const
{
    BlockSet blkR(8);
    auto i = GetX(id);
    auto j = GetY(id);
    for (auto di = -1; di <= 1; ++di)
        if (i + di >= 0 && i + di < m_Game.Width)
            for (auto dj = -1; dj <= 1; ++dj)
                if (j + dj >= 0 && j + dj < m_Game.Height)
                    if (di != 0 || dj != 0)
                        blkR.push_back(GetIndex(i + di, j + dj));
    return blkR;
}

void MWSolver::OpenBlock(int id)
{
    if (m_IsOpen[id])
        return;

    m_IsOpen[id] = true;

    if (m_Game.IsMine[id])
    {
        m_WrongGuesses++;
        AddRestrain(id, true);
        return;
    }

    AddRestrain(id, false);

    auto surr = GetBlockR(id);

    auto degree = 0;
    for (auto blk : surr)
        if (m_Game.IsMine[blk])
            degree++;

    if (degree == 0)
    {
        for (auto blk : surr)
            OpenBlock(blk);
    }
    AddRestrain(surr, degree);

    m_ToOpen--;
}

int MWSolver::RandomNonTrivial()
{
    m_Random_Temp.clear();
    m_Random_Temp.reserve(m_Game.Width * m_Game.Height);

    for (auto blk = 0; blk < m_Game.Width * m_Game.Height; blk++)
        if (!m_IsOpen[blk] && !BlockIsMine(blk))
            m_Random_Temp.push_back(blk);

    std::uniform_int_distribution<> dist(0, static_cast<int>(m_Random_Temp.size()) - 1);

    return dist(m_Random);
}
