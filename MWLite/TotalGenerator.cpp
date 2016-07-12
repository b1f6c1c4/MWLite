#include "TotalGenerator.h"
#include <algorithm>

TotalGenerator::TotalGenerator(int width, int height, int total) : RandomGenerator(width, height), m_Total(total) {}

TotalGenerator::~TotalGenerator() {}

void TotalGenerator::Fill(bool *data)
{
    memset(data, false, sizeof(*data) * m_Width * m_Height);

    for (auto i = 0; i < m_Total; i++)
        data[i] = true;

    std::shuffle(data, data + m_Width * m_Height, m_Random);
}
