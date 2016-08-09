#pragma once
#include "../stdafx.h"

#define USE_DENSE_SET

#ifdef USE_DENSE_SET
#include "BlockSet_Dense.h"
#else
#include "BlockSet_Sparse.h"
#endif

template <typename T>
Set<T> &operator+(Set<T> lhs, const Set<T> &rhs)
{
    return lhs += rhs;
}

template <typename T>
Set<T> &operator-(Set<T> lhs, const Set<T> &rhs)
{
    return lhs -= rhs;
}

template <typename T>
Set<T> &operator*(Set<T> lhs, const Set<T> &rhs)
{
    return lhs *= rhs;
}

typedef Set<Block> BlockSet;
