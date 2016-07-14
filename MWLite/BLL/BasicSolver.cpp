#include "BasicSolver.h"
#include <algorithm>
#include <map>

BasicSolver::BasicSolver(const Game &game) : MWSolver(game), m_Manager(game.Width * game.Height, BlockStatus::Unknown)
{
    m_BlockSets.emplace_back(game.Width * game.Height);
    auto &lst = m_BlockSets.back();
    for (auto i = 0; i < game.Width * game.Height; ++i)
        lst[i] = i;
    m_SetIDs.resize(game.Width * game.Height, 0);
    m_Matrix.emplace_back();

    if (game.TotalMines >= 0)
    {
        m_Matrix.back().push_back(1);
        m_MatrixAugment.push_back(game.TotalMines);
    }
}

bool BasicSolver::BlockIsMine(Block blk) const
{
    return m_Manager[blk] == BlockStatus::Mine;
}

BasicSolver::~BasicSolver() { }

void BasicSolver::AddRestrain(Block blk, bool isMine)
{
    if (m_Manager[blk] == BlockStatus::Unknown)
    {
        m_Manager[blk] = isMine ? BlockStatus::Mine : BlockStatus::Blank;
        if (m_SetIDs[blk] >= 0)
            ReduceBlockSet(m_SetIDs[blk]);
        return;
    }
    if (m_Manager[blk] == BlockStatus::Blank && isMine)
        throw;
    if (m_Manager[blk] == BlockStatus::Mine && !isMine)
        throw;
}

void BasicSolver::AddRestrain(const BlockSet &set, int mines)
{
    auto dMines = 0;
    auto &bin = m_IntersectionCounts_Temp;
    GetIntersectionCounts(set, bin, dMines);

    if (mines == dMines)
    {
        for (auto blk : set)
            if (m_Manager[blk] == BlockStatus::Unknown)
                m_Manager[blk] = BlockStatus::Blank;
        for (auto blk : set)
            if (m_SetIDs[blk] >= 0)
                ReduceBlockSet(m_SetIDs[blk]);
        return;
    }
    {
        auto t = 0;
        for (auto v : bin)
            t += v;
        if (mines == t + dMines)
        {
            for (auto blk : set)
                if (m_Manager[blk] == BlockStatus::Unknown)
                {
                    m_Manager[blk] = BlockStatus::Mine;
                }
            for (auto blk : set)
                if (m_SetIDs[blk] >= 0)
                    ReduceBlockSet(m_SetIDs[blk]);
            return;
        }
    }

    for (auto &containers : m_Matrix)
        containers.push_back(CONT_ZERO);

    for (auto col = 0; col < bin.size(); ++col)
    {
        if (bin[col] == 0)
            continue;

        if (bin[col] == m_BlockSets[col].size())
        {
            SB(m_Matrix[CNT(col)].back(), SHF(col));
            continue;
        }

        m_BlockSets.emplace_back();
        auto &it = m_BlockSets.back();
        it.reserve(bin[col]);

        auto p = 0, q = 0;
        for (; p < set.size() && q < m_BlockSets[col].size();)
        {
            if (set[p] < m_BlockSets[col][q])
                ++p;
            else if (set[p] > m_BlockSets[col][q])
                ++q;
            else
            {
                it.push_back(set[p]);
                m_BlockSets[col].erase(m_BlockSets[col].begin() + q);
                m_SetIDs[set[p++]] = static_cast<int>(m_BlockSets.size()) - 1;
            }
        }

        if (CONTS(m_BlockSets.size()) > m_Matrix.size())
        {
            ASSERT(CONTS(m_BlockSets.size()) == m_Matrix.size() + 1);
            m_Matrix.emplace_back(m_Matrix.front().size(), CONT_ZERO);
        }
        for (auto i = 0; i < m_Matrix[CNT(col)].size() - 1; ++i)
            if (NZ(m_Matrix[CNT(col)][i], SHF(col)))
            SB(m_Matrix[CNT(m_BlockSets.size() - 1)][i], SHF(m_BlockSets.size() - 1));
        SB(m_Matrix[CNT(m_BlockSets.size() - 1)].back(), SHF(m_BlockSets.size() - 1));
    }

    m_MatrixAugment.push_back(mines - dMines);
}

int BasicSolver::NextBlock(const bool *cancelToken)
{
    Simplify(cancelToken);
    if (*cancelToken)
        return -1;

    MergeSets(cancelToken);
    if (*cancelToken)
        return -1;

    for (auto blk = 0; blk < m_Manager.size(); ++blk)
        if (!IsOpen(blk) && m_Manager[blk] == BlockStatus::Blank)
            return blk;

    return -1;
}

void BasicSolver::GetIntersectionCounts(const BlockSet &set1, std::vector<int> &sets1, int &mines) const
{
    sets1.clear();
    sets1.resize(m_BlockSets.size(), 0);
    mines = 0;
    for (auto blk : set1)
    {
        auto v = m_SetIDs[blk];
        if (v == -1)
        {
            ++mines;
            continue;
        }
        if (v == -2)
            continue;
        ++sets1[m_SetIDs[blk]];
    }
}

void BasicSolver::DropColumn(int col)
{
#ifdef _DEBUG
    for (auto blk : m_BlockSets[col])
    ASSERT(m_SetIDs[blk] != col);
#endif
    auto last = m_BlockSets.size() - 1;
    if (col != last)
        m_BlockSets.back().swap(m_BlockSets[col]);
    m_BlockSets.pop_back();
    if (col != last)
    {
        for (auto blk : m_BlockSets[col])
        {
            ASSERT(m_Manager[blk] != BlockStatus::Unknown || m_SetIDs[blk] == last);
            m_SetIDs[blk] = col;
        }
        for (auto j = 0; j < m_Matrix[CNT(last)].size(); ++j)
            if (NZ(m_Matrix[CNT(last)][j], SHF(last)))
            SB(m_Matrix[CNT(col)][j], SHF(col));
            else
            CB(m_Matrix[CNT(col)][j], SHF(col));
    }
    for (auto j = 0; j < m_Matrix[CNT(last)].size(); ++j)
    CB(m_Matrix[CNT(last)][j], SHF(last));
    if (last % CONT_SIZE == 0)
        m_Matrix.pop_back();
}

void BasicSolver::DropRow(int row)
{
    if (row != m_MatrixAugment.size() - 1)
    {
        m_MatrixAugment[row] = m_MatrixAugment.back();
        m_MatrixAugment.pop_back();
        for (auto &containers : m_Matrix)
        {
            containers[row] = containers.back();
            containers.pop_back();
        }
    }
    else
    {
        m_MatrixAugment.pop_back();
        for (auto &containers : m_Matrix)
            containers.pop_back();
    }
}

void BasicSolver::MergeSets(const bool *cancelToken)
{
    std::multimap<size_t, int> hash;
    for (auto i = 0; i < m_BlockSets.size(); ++i)
    {
        if (*cancelToken)
            return;

        size_t h = 5381;
        for (auto v : m_Matrix[CNT(i)])
            h = (h << 5) + h + B(v, SHF(i));
        auto itp = hash.equal_range(h);
        auto flag = true;
        for (auto it = itp.first; it != itp.second; ++it)
        {
            for (auto j = 0; j < m_Matrix[CNT(i)].size(); ++j)
                if (B(m_Matrix[CNT(i)][j], SHF(i)) != B(m_Matrix[CNT(it->second)][j], SHF(it->second)))
                {
                    flag = false;
                    break;
                }
            if (flag)
            {
                m_BlockSets[it->second].reserve(m_BlockSets[it->second].size() + m_BlockSets[i].size());
                for (auto &blk : m_BlockSets[i])
                    m_BlockSets[it->second].push_back(blk);
                sort(m_BlockSets[it->second].begin(), m_BlockSets[it->second].end());
                for (auto &blk : m_BlockSets[i])
                {
                    ASSERT(m_Manager[blk] != BlockStatus::Unknown || m_SetIDs[blk] == i);
                    m_SetIDs[blk] = it->second;
                }
                DropColumn(i);

                --i;
                break;
            }
        }
        if (flag)
            hash.insert(std::make_pair(h, i));
    }
}

bool BasicSolver::ReduceBlockSet(int col)
{
    auto dMines = 0;
    auto &setN = m_Reduce_Temp;
    setN.clear();
    for (auto it = m_BlockSets[col].begin(); it != m_BlockSets[col].end(); ++it)
    {
        switch (m_Manager[*it])
        {
        case BlockStatus::Mine:
            ++dMines;
            m_SetIDs[*it] = -1;
            break;
        case BlockStatus::Blank:
            m_SetIDs[*it] = -2;
            break;
        case BlockStatus::Unknown:
            ASSERT(m_SetIDs[*it] == col);
            setN.push_back(*it);
            break;
        default:
            ASSERT(false);
        }
    }
    setN.swap(m_BlockSets[col]);
    if (dMines != 0)
    {
        for (auto j = 0; j < m_Matrix[CNT(col)].size(); ++j)
            if (NZ(m_Matrix[CNT(col)][j], SHF(col)))
            {
                m_MatrixAugment[j] -= dMines;
                ASSERT(m_MatrixAugment[j] >= 0);
            }
    }
    if (m_BlockSets[col].empty())
    {
        DropColumn(col);
        return true;
    }
    return false;
}
