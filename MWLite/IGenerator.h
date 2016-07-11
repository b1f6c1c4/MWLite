#pragma once
#include "stdafx.h"
#include "Game.h"

class IGenerator
{
public:
    virtual ~IGenerator() { }

    virtual Game GenerateGame() = 0;

protected:
    IGenerator() { }
};
