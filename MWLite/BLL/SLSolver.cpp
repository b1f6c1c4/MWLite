#include "SLSolver.h"

SLSolver::SLSolver(const Game &game) : BasicSolver(game) { }

SLSolver::~SLSolver() { }

void SLSolver::Simplify()
{
    ReduceRestrains();
}

bool SLSolver::ReduceRestrainBlank(int row)
{
    ASSERT(m_MatrixAugment[row] >= 0);
    if (m_MatrixAugment[row] != 0)
        return false;

    for (auto col = 0; col < m_BlockSets.size(); ++col)
    {
        if (Z(m_Matrix[CNT(col)][row], SHF(col)))
            continue;

        for (auto blk : m_BlockSets[col])
        {
            ASSERT(m_Manager[blk] == BlockStatus::Blank || m_SetIDs[blk] == col);
            m_SetIDs[blk] = -2;
            if (m_Manager[blk] == BlockStatus::Blank)
                continue;
            ASSERT(m_Manager[blk] == BlockStatus::Unknown);
            m_Manager[blk] = BlockStatus::Blank;
        }
        DropColumn(col--);
    }
    DropRow(row);
    return true;
}

bool SLSolver::ReduceRestrainMine(int row)
{
    auto &sum = m_ReduceCount_Temp;
    ASSERT(m_MatrixAugment[row] <= sum[row]);
    if (m_MatrixAugment[row] != sum[row])
        return false;

    for (auto col = 0; col < m_BlockSets.size(); ++col)
    {
        if (Z(m_Matrix[CNT(col)][row], SHF(col)))
            continue;

        for (auto j = 0; j < m_Matrix[CNT(col)].size(); ++j)
            if (NZ(m_Matrix[CNT(col)][j], SHF(col)))
            {
                m_MatrixAugment[j] -= static_cast<int>(m_BlockSets[col].size());
                sum[j] -= m_BlockSets[col].size();
                ASSERT(m_MatrixAugment[j] >= 0);
            }
        for (auto blk : m_BlockSets[col])
        {
            ASSERT(m_Manager[blk] == BlockStatus::Mine || m_SetIDs[blk] == col);
            m_SetIDs[blk] = -1;
            if (m_Manager[blk] == BlockStatus::Mine)
                continue;
            ASSERT(m_Manager[blk] == BlockStatus::Unknown);
            m_Manager[blk] = BlockStatus::Mine;
        }
        DropColumn(col--);
    }
    if (row != sum.size() - 1)
        sum[row] = sum.back();
    sum.pop_back();
    DropRow(row);
    return true;
}

void SLSolver::ReduceRestrains()
{
    for (auto col = 0; col < m_BlockSets.size(); ++col)
        if (ReduceBlockSet(col))
            --col;

    for (auto row = 0; row < m_MatrixAugment.size(); ++row)
        if (ReduceRestrainBlank(row))
            --row;

    if (m_Matrix.empty())
    {
        m_MatrixAugment.clear();
        return;
    }

    auto &sum = m_ReduceCount_Temp;
    sum.clear();
    sum.resize(m_Matrix.front().size(), CONT_ZERO);
    for (auto cnt = 0; cnt < m_Matrix.size(); ++cnt)
        for (auto i = 0; i < m_Matrix[cnt].size(); ++i)
        {
            auto v = m_Matrix[cnt][i];
            for (auto shift = 0; shift < CONT_WIDTH(m_Matrix, cnt); ++shift , v >>= 1)
                if (NZ(v, 0))
                    sum[i] += m_BlockSets[cnt * CONT_SIZE + shift].size();
                else if (v == 0)
                    break;
        }

    for (auto row = 0; row < m_MatrixAugment.size(); ++row)
        if (ReduceRestrainMine(row))
            --row;
}
