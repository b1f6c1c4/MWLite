#pragma once
#include "../stdafx.h"
#include "BlockSet.h"
#include "Configuration.h"

struct Game
{
    Game() : AllMines(0) { }

    std::shared_ptr<Configuration> Config;

    DEFAULT_COPY(Game);
    DEFAULT_MOVE(Game);

    BlockSet AllMines; // FullSize = Width * Height
};
