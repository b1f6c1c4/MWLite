#pragma once
#include "../stdafx.h"
#include "../Entities/Game.h"
#include "ISolver.h"
#include "RandomChooser.h"
#include <functional>
#include "IGenerator.h"
#include "Strategies/SolverBuilder.h"

class Simulator
{
public:
    Simulator(std::shared_ptr<Configuration> config, std::shared_ptr<IGenerator> gameGen, std::shared_ptr<SolverBuilder> solverGen, bool imme);
    virtual ~Simulator();

    NO_COPY(Simulator);
    NO_MOVE(Simulator);

    int Simulate(const CancellationToken &cancel);

private:
    std::shared_ptr<Configuration> m_Config;
    std::shared_ptr<IGenerator> m_GameGen;
    std::shared_ptr<SolverBuilder> m_SolverGen;
    bool m_Imme;

    std::shared_ptr<Game> m_Game;
    std::vector<SparseBlockSet> m_DeletedNeighborhood;
    std::vector<size_t> m_NeighborCount;

    std::shared_ptr<ISolver> m_Solver;

    std::shared_ptr<BlockSet> ClosedBlocks;

    std::shared_ptr<BlockSet> OpenMines;

    std::shared_ptr<BlockSet> OpenNoMines;

    int m_ToOpen;

    int GetIndex(int x, int y) const;
    int GetX(int id) const;
    int GetY(int id) const;

    void ResetToOpen();
    void ResetNeighborCount();
};
