#pragma once
#include "../stdafx.h"

#pragma pack(push, 8)
struct Configuration
{
    // generator settings
    int Width;
    int Height;

    bool UseTotalMines;
    int TotalMines;
    double Probability;

    // solver settings
    bool DisableDual;
};
#pragma pack(pop)
