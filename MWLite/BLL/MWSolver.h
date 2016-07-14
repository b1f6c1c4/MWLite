#pragma once
#include "../stdafx.h"
#include "../Entities/Game.h"
#include <vector>
#include <random>

typedef int Block;
typedef std::vector<Block> BlockSet;

class MWSolver
{
public:
    explicit MWSolver(const Game &game);
    virtual ~MWSolver();

    MWSolver(const MWSolver &other) = delete;
    MWSolver(MWSolver &&other) = delete;
    MWSolver &operator=(const MWSolver &other) = delete;
    MWSolver &operator=(MWSolver &&other) = delete;

    int Solve(const bool *cancelToken);

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
