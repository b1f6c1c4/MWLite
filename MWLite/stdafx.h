#pragma once
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

#ifdef MINESWEEPERSOLVER_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#ifdef _DEBUG
#define ASSERT(val) do { if (!(val)) throw; } while (false)
#else
#define ASSERT(val)
#endif

#include "../../CppUtil/CppUtil/CopyMove.hpp"

#include <vector>
#include <memory>

typedef int_fast32_t Block;
