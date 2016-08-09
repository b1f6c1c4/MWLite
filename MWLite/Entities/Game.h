#pragma once
#include "../stdafx.h"
#include "BlockSet.h"

struct Game
{
    int Width;
    int Height;

    int TotalMines; // < 0 when unavaliable

    BlockSet AllMines; // FullSize = Width * Height

    Game(int width, int height);
};

inline Game::Game(int width, int height) : Width(width), Height(height), TotalMines(-1), AllMines(width * height) { }
