#pragma once
#include "../stdafx.h"
#include "IGenerator.h"
#include <random>

class RandomGenerator : public IGenerator
{
public:
    ~RandomGenerator();

    void GenerateGame(Game &game) override;
protected:
    RandomGenerator(int width, int height);

    std::mt19937_64 m_Random;

    int m_Width, m_Height;

    virtual void Fill(bool *data) = 0;
};
