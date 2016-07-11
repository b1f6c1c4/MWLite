#pragma once
#include "stdafx.h"

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
