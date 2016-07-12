#include "RandomGenerator.h"

RandomGenerator::RandomGenerator(int width, int height) : m_Width(width), m_Height(height)
{
    m_Random.seed(std::random_device()());
}

RandomGenerator::~RandomGenerator() {}

void RandomGenerator::GenerateGame(Game &game)
{
    if (game.Width != m_Width || game.Height != m_Height)
    {
        game.Width = m_Width;
        game.Height = m_Height;
        if (game.IsMine != nullptr)
        {
            delete[] game.IsMine;
            game.IsMine = nullptr;
        }
    }

    Fill(game.IsMine);
}