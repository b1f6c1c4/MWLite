#pragma once
#include "../stdafx.h"
#include "SparseSet.h"
#include "DenseSet.h"

#define USE_DENSE_SET

#ifdef USE_DENSE_SET
template <typename T>
using set_t = DenseSet<T>;
#else
template <typename T>
using set_t = SparseSet<T>;
#endif

typedef set_t<Block> BlockSet;
