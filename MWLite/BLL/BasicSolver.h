#pragma once
#include "../stdafx.h"
#include "MWSolver.h"

enum class BlockStatus
{
    Unknown = -127,
    Mine = -1,
    Blank = -2
};

typedef size_t Container;

#define CONT_ZERO static_cast<Container>(0)
#define CONT_ONE static_cast<Container>(1)
#define CONT_SIZE (sizeof(Container) * 8)
#define CONTS(offset) (((offset) + CONT_SIZE - 1) / CONT_SIZE)
#define CNT(offset) ((offset) / CONT_SIZE)
#define SHF(offset) ((offset) % CONT_SIZE)
#define MASK(shift) (CONT_ONE << (shift))
#define MASKL(lng) (MASK((lng)) - CONT_ONE)
#define MASKH(lng) (~(MASK(CONT_SIZE - (lng)) - CONT_ONE))
#define LB1(val) (MASK(0) & (val))
#define HB1(val) ((MASK(CONT_SIZE - 1) & (val)) >> (CONT_SIZE - 1))
#define LB(val, lng) (MASKL((lng)) & (val))
#define HB(val, lng) ((MASKH((lng)) & (val)) >> (CONT_SIZE - (lng)))
#define B(val, shift) ((MASK((shift)) & (val)) >> (shift))
#define Z(val, shift) (B((val), (shift)) == CONT_ZERO)
#define NZ(val, shift) (B((val), (shift)) != CONT_ZERO)
#define SB(lval, shift) (lval) |= MASK((shift))
#define CB(lval, shift) (lval) &= ~MASK((shift))

#define CONT_WIDTH(lst, cnt) (cnt == lst.size() - 1 && SHF(m_BlockSets.size()) > 0 ? SHF(m_BlockSets.size()) : CONT_SIZE)

class BasicSolver : public MWSolver
{
public:
    ~BasicSolver();

protected:
    explicit BasicSolver(const Game &game);

    bool BlockIsMine(Block blk) const override;

    void AddRestrain(Block blk, bool isMine) override;
    void AddRestrain(const BlockSet &set, int mines) override;
    int NextBlock(const bool *cancelToken) override;

    virtual void Simplify(const bool *cancelToken) = 0;

    std::vector<BlockStatus> m_Manager;
    std::vector<BlockSet> m_BlockSets;
    std::vector<int> m_SetIDs;
    std::vector<std::vector<Container>> m_Matrix;
    std::vector<int> m_MatrixAugment;
    std::vector<int> m_Minors;

    void GetIntersectionCounts(const BlockSet &set1, std::vector<int> &sets1, int &mines) const;

    void DropColumn(int col);
    void DropRow(int row);

    bool ReduceBlockSet(int col);

private:
    BlockSet m_Reduce_Temp;

    std::vector<int> m_IntersectionCounts_Temp;

    void MergeSets(const bool *cancelToken);
};
