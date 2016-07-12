#pragma once
#include "../stdafx.h"
#include "../Entities/Game.h"

class ISolver
{
public:
    virtual ~ISolver() { }

    virtual int Process(const Game &game) = 0;

protected:
    ISolver() { }
};
