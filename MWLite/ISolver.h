#pragma once
#include "stdafx.h"
#include "Game.h"

class ISolver
{
public:
    virtual ~ISolver() { }

    virtual int Process(const Game &game) = 0;

protected:
    ISolver() { }
};
