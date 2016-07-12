#pragma once
#include "../stdafx.h"
#include <type_traits>

struct Game
{
    int Width;
    int Height;

    int TotalMines; // < 0 when unavaliable

    bool *IsMine; // bool [Width * Height]

    Game(int width, int height);
    Game(const Game &other);
    Game(Game &&other);
    ~Game();

    Game &operator=(const Game &other);
    Game &operator=(Game &&other);
};

inline Game::Game(int width, int height) : Width(width), Height(height), TotalMines(-1), IsMine() { }

inline Game::Game(const Game &other)
{
    *this = other;
}

inline Game::Game(Game &&other)
{
    *this = std::move(other);
}

inline Game::~Game()
{
    if (IsMine != nullptr)
    {
        delete[] IsMine;
        IsMine = nullptr;
    }
}

inline Game &Game::operator=(const Game &other)
{
    if (this == &other)
        return *this;

    if (IsMine != nullptr)
    {
        delete[] IsMine;
        IsMine = nullptr;
    }

    Width = other.Width;
    Height = other.Height;
    TotalMines = other.TotalMines;

    if (other.IsMine == nullptr)
        return *this;
    IsMine = new bool[Width * Height];
    memcpy(IsMine, other.IsMine, Width * Height);

    return *this;
}

inline Game &Game::operator=(Game &&other)
{
    if (this == &other)
        return *this;

    if (IsMine != nullptr)
    {
        delete[] IsMine;
        IsMine = nullptr;
    }

    Width = other.Width;
    Height = other.Height;
    TotalMines = other.TotalMines;

    IsMine = other.IsMine;

    if (other.IsMine == nullptr)
        return *this;

    delete[] other.IsMine;
    other.IsMine = nullptr;

    return *this;
}
