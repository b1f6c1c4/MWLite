#pragma once
#include "../stdafx.h"
#include "../Entities/Game.h"
#include "ISolver.h"
#include "RandomChooser.h"
#include <functional>

class Simulator
{
public:
    explicit Simulator(const Game &game, std::shared_ptr<ISolver> slv);
    virtual ~Simulator();

    NO_COPY(Simulator);
    NO_MOVE(Simulator);

    int Solve(const CancellationToken &cancel, std::function<void(int)> adjuster);

private:
    const Game &m_Game;
    std::vector<SparseBlockSet> m_DeletedNeighorhood;

    std::shared_ptr<ISolver> m_Solver;

    std::shared_ptr<BlockSet> ClosedBlocks;

    std::shared_ptr<BlockSet> OpenMines;

    std::shared_ptr<BlockSet> OpenNoMines;

    int m_ToOpen;

    int GetIndex(int x, int y) const;
    int GetX(int id) const;
    int GetY(int id) const;

    void ResetToOpen();
};
