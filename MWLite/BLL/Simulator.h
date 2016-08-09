#pragma once
#include "../stdafx.h"
#include "../Entities/Game.h"
#include <random>
#include <functional>

class Simulator
{
public:
    explicit Simulator(const Game &game);
    virtual ~Simulator();

    Simulator(const Simulator &other) = delete;
    Simulator(Simulator &&other) = delete;
    Simulator &operator=(const Simulator &other) = delete;
    Simulator &operator=(Simulator &&other) = delete;

    int Solve(const bool *cancelToken, std::function<void(int)> adjuster);

protected:
    bool IsOpen(int id) const;

    virtual bool BlockIsMine(Block blk) const = 0;

    virtual void AddRestrain(Block blk, bool isMine) = 0;
    virtual void AddRestrain(const BlockSet &set, int mines) = 0;
    virtual int NextBlock(const bool *cancelToken) = 0;

private:
    const Game &m_Game;
    std::vector<bool> m_IsOpen;

    std::mt19937_64 m_Random;
    BlockSet m_Random_Temp;

    int m_ToOpen, m_WrongGuesses;

    int GetIndex(int x, int y) const;
    int GetX(int id) const;
    int GetY(int id) const;
    BlockSet GetBlockR(int id) const;
    void OpenBlock(int id);
    int RandomNonTrivial();
};
