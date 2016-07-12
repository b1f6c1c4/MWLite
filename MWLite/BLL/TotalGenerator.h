#pragma once
#include "../stdafx.h"
#include "RandomGenerator.h"

class TotalGenerator : public RandomGenerator
{
public:
    TotalGenerator(int width, int height, int total);
    ~TotalGenerator();

protected:
    void Fill(bool *data) override;

private:
    int m_Total;
};
