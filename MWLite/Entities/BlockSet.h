#pragma once
#include "../stdafx.h"
#include "SparseSet.h"
#include "DenseSet.h"

#define USE_DENSE_SET

#ifdef USE_DENSE_SET
typedef DenseSet<Block> BlockSet;
#else
typedef SparseSet<Block> BlockSet;
#endif
