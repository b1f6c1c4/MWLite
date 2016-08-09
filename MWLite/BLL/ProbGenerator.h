#pragma once
#include "../stdafx.h"
#include "RandomGenerator.h"

class ProbGenerator : public RandomGenerator
{
public:
    ProbGenerator(int width, int height, double prob);
    ~ProbGenerator();

protected:
    void Fill(BlockSet &data) override;

private:
    double m_Prob;
};
