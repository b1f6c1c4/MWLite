#pragma once
#include "stdafx.h"

struct Game
{
    int Width;
    int Height;

    int TotalMines; // < 0 when unavaliable

    bool *IsMine; // bool [Width * Height]
};
