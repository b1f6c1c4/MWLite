#include "NotRigorousGenerator.h"

NotRigorousGenerator::NotRigorousGenerator(std::shared_ptr<IGenerator> base) : m_Base(base) { }

NotRigorousGenerator::~NotRigorousGenerator() { }

void NotRigorousGenerator::GenerateGame(Game &game)
{
    m_Base->GenerateGame(game);
}

void NotRigorousGenerator::AdjustGame(Game &game, int initial)
{
    while (game.AllMines[initial])
        m_Base->GenerateGame(game);
}
