#pragma once
#include "../stdafx.h"
#include "SparseSet.h"
#include "DenseSet.h"
#include "DenseBitSet.h"

template <typename T>
using set_t = DenseBitSet<T>;

typedef set_t<Block> BlockSet;

typedef SparseSet<Block> SparseBlockSet;


inline SparseBlockSet operator*(const SparseBlockSet &lhs, const BlockSet &rhs)
{
    SparseBlockSet set(lhs.FullSize());
    for (auto b : lhs)
        if (rhs[b])
            set += b;
    return set;
}

inline SparseBlockSet operator-(const SparseBlockSet &lhs, const BlockSet &rhs)
{
    SparseBlockSet set(lhs.FullSize());
    for (auto b : lhs)
        if (!rhs[b])
            set += b;
    return set;
}

inline BlockSet &operator+=(BlockSet &lhs, const SparseBlockSet &rhs)
{
    for (auto b : rhs)
        lhs += b;
    return lhs;
}

inline BlockSet &operator-=(BlockSet &lhs, const SparseBlockSet &rhs)
{
    for (auto b : rhs)
        lhs += b;
    return lhs;
}

inline BlockSet &operator*=(BlockSet &lhs, const SparseBlockSet &rhs)
{
    return lhs = BlockSet(rhs * lhs);
}

inline BlockSet operator+(BlockSet lhs, const SparseBlockSet &rhs)
{
    return lhs += rhs;
}

inline BlockSet operator-(BlockSet lhs, const SparseBlockSet &rhs)
{
    return lhs -= rhs;
}

inline BlockSet operator*(BlockSet lhs, const SparseBlockSet &rhs)
{
    return lhs *= rhs;
}

