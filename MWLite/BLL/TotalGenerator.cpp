#include "TotalGenerator.h"
#include <algorithm>

TotalGenerator::TotalGenerator(int width, int height, int total) : RandomGenerator(width, height), m_Total(total) {}

TotalGenerator::~TotalGenerator() {}

void TotalGenerator::Fill(BlockSet &data)
{
    std::vector<int_fast8_t> set(data.FullSize(), false);
    for (auto i = 0; i < m_Total; i++)
        set[i] = true;

    std::shuffle(set.begin(), set.end(), m_Random);

    for (auto i = 0; i < data.FullSize(); i++)
        if (set[i])
            data += i;
}
