#pragma once
#include "../stdafx.h"

#pragma pack(push, 8)
struct Configuration
{
    int Width;
    int Height;

    bool UseTotalMines;
    int TotalMines;
    double Probability;

    bool NotRigorous;

    size_t Mines() const
    {
        return Width * Height;
    }
};
#pragma pack(pop)

enum class LogicLevel
{
    ZeroLogic = 0,
    PassiveLogic = 1,
    SingleLogic = 2,
    SingleLogicExtended = 3,
    DoubleLogic = 4,
    DoubleLogicExtended = 5,
    FullLogic = 6
};
