#pragma once
#include "../stdafx.h"
#include <memory>
#include "IGenerator.h"

class NotRigorousGenerator : public IGenerator
{
public:
    explicit NotRigorousGenerator(std::shared_ptr<IGenerator> base);
    ~NotRigorousGenerator();

    void GenerateGame(Game &game) override;

    virtual void AdjustGame(Game &game, int initial) override;

private:
    std::shared_ptr<IGenerator> m_Base;
};
