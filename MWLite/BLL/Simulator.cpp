#include "Simulator.h"

Simulator::Simulator(const Game &game) : m_Game(game) , m_ToOpen(game.Width * game.Height), m_WrongGuesses(0), m_Random_Temp(game.AllMines.FullSize())
{
    m_IsOpen.resize(game.Width * game.Height, false);

    m_Random.seed(std::random_device()());

    if (game.TotalMines >= 0)
    {
        m_ToOpen -= game.TotalMines;
        return;
    }

    for (auto i = 0; i < game.Width * game.Height; i++)
        if (game.AllMines[i])
            m_ToOpen--;
}

Simulator::~Simulator() { }

int Simulator::Solve(const bool *cancelToken, std::function<void(int)> generator)
{
    auto flag = true;
    while (m_ToOpen > 0)
    {
        if (*cancelToken)
            return -1;

        auto blk = NextBlock(cancelToken);
        if (blk < 0)
            blk = RandomNonTrivial();

        if (flag)
        {
            generator(blk);

            m_ToOpen = m_Game.Width * m_Game.Height;
            if (m_Game.TotalMines >= 0)
                m_ToOpen -= m_Game.TotalMines;
            else
                for (auto i = 0; i < m_Game.Width * m_Game.Height; i++)
                    if (m_Game.AllMines[i])
                        m_ToOpen--;

            flag = false;
        }
        OpenBlock(blk);
    }
    return m_WrongGuesses;
}

bool Simulator::IsOpen(int id) const
{
    return m_IsOpen[id];
}

int Simulator::GetIndex(int x, int y) const
{
    return x * m_Game.Height + y;
}

int Simulator::GetX(int id) const
{
    return id / m_Game.Height;
}

int Simulator::GetY(int id) const
{
    return id % m_Game.Height;
}

BlockSet Simulator::GetBlockR(int id) const
{
    BlockSet blkR(m_Game.AllMines.FullSize());
    auto i = GetX(id);
    auto j = GetY(id);
    for (auto di = -1; di <= 1; ++di)
        if (i + di >= 0 && i + di < m_Game.Width)
            for (auto dj = -1; dj <= 1; ++dj)
                if (j + dj >= 0 && j + dj < m_Game.Height)
                    if (di != 0 || dj != 0)
                        blkR += GetIndex(i + di, j + dj);
    return blkR;
}

void Simulator::OpenBlock(int id)
{
    if (m_IsOpen[id])
        return;

    m_IsOpen[id] = true;

    if (m_Game.AllMines[id])
    {
        m_WrongGuesses++;
        AddRestrain(id, true);
        return;
    }

    AddRestrain(id, false);

    auto surr = GetBlockR(id);

    auto degree = 0;
    for (auto blk : surr)
        if (m_Game.AllMines[blk])
            degree++;

    if (degree == 0)
    {
        for (auto blk : surr)
            OpenBlock(blk);
    }
    AddRestrain(surr, degree);

    m_ToOpen--;
}

int Simulator::RandomNonTrivial()
{
    m_Random_Temp.Clear();

    for (auto blk = 0; blk < m_Game.Width * m_Game.Height; blk++)
        if (!m_IsOpen[blk] && !BlockIsMine(blk))
            m_Random_Temp += blk;

    std::uniform_int_distribution<> dist(0, static_cast<int>(m_Random_Temp.Count()) - 1);

    return m_Random_Temp[dist(m_Random)];
}
