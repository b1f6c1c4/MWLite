#pragma once
#include "../stdafx.h"

#pragma pack(push, 8)
struct BaseConfiguration
{
    int Width;
    int Height;

    bool UseTotalMines;
    int TotalMines;
    double Probability;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct Configuration : BaseConfiguration
{
    bool NotRigorous;

    bool DisableDual;
};
#pragma pack(pop)
