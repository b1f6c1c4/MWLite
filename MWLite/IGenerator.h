#pragma once
#include "stdafx.h"
#include "Game.h"

class IGenerator
{
public:
    virtual ~IGenerator() { }

    virtual void GenerateGame(Game &game) = 0;

protected:
    IGenerator() { }
};
