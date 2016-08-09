#pragma once
#include "../stdafx.h"
#include "../Entities/Game.h"

class IGenerator
{
public:
    virtual ~IGenerator() { }

    virtual void GenerateGame(Game &game) = 0;

    virtual void AdjustGame(Game &game, int initial) = 0;

protected:
    IGenerator() { }
};
