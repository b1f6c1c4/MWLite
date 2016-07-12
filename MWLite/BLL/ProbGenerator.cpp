#include "ProbGenerator.h"
#include <random>

ProbGenerator::ProbGenerator(int width, int height, double prob) : RandomGenerator(width, height), m_Prob(prob) {}

ProbGenerator::~ProbGenerator() {}

void ProbGenerator::Fill(bool *data)
{
    std::bernoulli_distribution dist(m_Prob);
    for (auto i = 0; i < m_Width * m_Height; i++)
        data[i] = dist(m_Random);
}
