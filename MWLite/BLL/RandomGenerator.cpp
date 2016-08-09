#include "RandomGenerator.h"

RandomGenerator::RandomGenerator(int width, int height) : m_Width(width), m_Height(height)
{
    m_Random.seed(std::random_device()());
}

RandomGenerator::~RandomGenerator() {}

void RandomGenerator::GenerateGame(Game &game)
{
    game.AllMines = BlockSet(m_Width * m_Height);

    Fill(game.AllMines);
}

void RandomGenerator::AdjustGame(Game &, int) { }
