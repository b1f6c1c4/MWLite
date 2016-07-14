#include "DLSolver.h"

DLSolver::DLSolver(const Game &game):SLSolver(game), m_Pairs_Temp(nullptr), m_Pairs_Temp_Size(0) { }

DLSolver::~DLSolver()
{
    if (m_Pairs_Temp != nullptr)
        delete[] m_Pairs_Temp;
}

void DLSolver::Simplify(const bool *cancelToken)
{
    while (!cancelToken)
    {
        ReduceRestrains(cancelToken);
        if (*cancelToken)
            return;

        if (!SimpleOverlapAll(cancelToken))
            break;
    }
}

bool DLSolver::SimpleOverlapAll(const bool *cancelToken)
{
    if (m_MatrixAugment.empty())
        return false;

    auto d = m_MatrixAugment.size();
    auto sz = (d - 1) * d / 2;
    if (m_Pairs_Temp != nullptr && m_Pairs_Temp_Size < sz)
    {
        delete[] m_Pairs_Temp;
        m_Pairs_Temp = nullptr;
    }
    if (m_Pairs_Temp == nullptr)
    {
        m_Pairs_Temp_Size = sz;
        m_Pairs_Temp = new bool[sz];
        ZeroMemory(m_Pairs_Temp, sizeof(bool) * m_Pairs_Temp_Size);
    }

    for (auto cnt = 0; cnt < m_Matrix.size(); ++cnt)
    {
        if (*cancelToken)
            return false;

        auto id = 0;
        for (auto p = 0; p < d - 1; ++p)
            for (auto q = p + 1; q < d; ++q , ++id)
                if (!m_Pairs_Temp[id])
                {
                    auto a = m_Matrix[cnt][p], b = m_Matrix[cnt][q];
                    if ((cnt == m_Matrix.size() - 1 ? LB(a & b, SHF(m_BlockSets.size())) : a & b) != CONT_ZERO)
                    {
                        if (SimpleOverlap(p, q))
                            return true;

                        m_Pairs_Temp[id] = true;
                    }
                }
        ASSERT(id == sz);
    }

    return false;
}

bool DLSolver::SimpleOverlap(int r1, int r2)
{
    auto &exceptA = m_OverlapA_Temp, &exceptB = m_OverlapB_Temp, &intersection = m_OverlapC_Temp;
    exceptA.clear() , exceptA.reserve(m_Matrix.size());
    exceptB.clear() , exceptB.reserve(m_Matrix.size());
    intersection.clear() , intersection.reserve(m_Matrix.size());


    for (auto &containers : m_Matrix)
    {
        exceptA.push_back(containers[r1] & ~containers[r2]);
        exceptB.push_back(~containers[r1] & containers[r2]);
        intersection.push_back(containers[r1] & containers[r2]);
    }

    typedef std::pair<int, int> Iv;

    auto sum = [this](const std::vector<Container> &lst)-> Iv
        {
            Iv iv(0, 0);
            for (auto cnt = 0; cnt < lst.size(); ++cnt)
            {
                auto v = lst[cnt];
                for (auto shift = 0; shift < CONT_WIDTH(lst, cnt); ++shift , v >>= 1)
                    if (NZ(v, 0))
                        iv.second += static_cast<int>(m_BlockSets[cnt * CONT_SIZE + shift].size());
                    else if (v == 0)
                        break;
            }
            return iv;
        };

    auto subs = [](Iv i1, Iv i2)-> Iv
        {
            return Iv(i1.first - i2.second, i1.second - i2.first);
        };
    auto ints = [](Iv i1, Iv i2)-> Iv
        {
            return Iv(max(i1.first, i2.first), min(i1.second, i2.second));
        };

    auto ivAC = Iv(m_MatrixAugment[r1], m_MatrixAugment[r1]), ivBC = Iv(m_MatrixAugment[r2], m_MatrixAugment[r2]);
    auto ivA0 = sum(exceptA), ivB0 = sum(exceptB), ivC0 = sum(intersection);
    auto ivA = ivA0, ivB = ivB0, ivC = ivC0;
    ivA = ints(ivA, subs(ivAC, ivC));
    ivB = ints(ivB, subs(ivBC, ivC));
    ivC = ints(ivC, ints(subs(ivAC, ivA), subs(ivBC, ivB)));
    ivA = ints(ivA, subs(ivAC, ivC));
    ivB = ints(ivB, subs(ivBC, ivC));
    ivC = ints(ivC, ints(subs(ivAC, ivA), subs(ivBC, ivB)));

    auto proc = [this](const std::vector<Container> &lst, const Iv &iv0, const Iv &iv)
        {
            if (lst.empty())
                return;
            if (iv0.second == iv.first)
                for (auto cnt = 0; cnt < lst.size(); ++cnt)
                {
                    auto v = lst[cnt];
                    for (auto shift = 0; shift < CONT_WIDTH(lst, cnt); ++shift , v >>= 1)
                        if (NZ(v, 0))
                        {
                            for (const auto &blk : m_BlockSets[cnt * CONT_SIZE + shift])
                            {
                                if (m_Manager[blk] == BlockStatus::Mine)
                                    continue;
                                ASSERT(m_Manager[blk] == BlockStatus::Unknown);
                                m_Manager[blk] = BlockStatus::Mine;
                            }
                        }
                        else if (v == 0)
                            break;
                }
            else if (iv0.first == iv.second)
                for (auto cnt = 0; cnt < lst.size(); ++cnt)
                {
                    auto v = lst[cnt];
                    for (auto shift = 0; shift < CONT_WIDTH(lst, cnt); ++shift , v >>= 1)
                        if (NZ(v, 0))
                        {
                            for (const auto &blk : m_BlockSets[cnt * CONT_SIZE + shift])
                            {
                                if (m_Manager[blk] == BlockStatus::Blank)
                                    continue;
                                ASSERT(m_Manager[blk] == BlockStatus::Unknown);
                                m_Manager[blk] = BlockStatus::Blank;
                            }
                        }
                        else if (v == 0)
                            break;
                }
        };

    proc(exceptA, ivA0, ivA);
    proc(exceptB, ivB0, ivB);
    proc(intersection, ivC0, ivC);

    return false;
}
