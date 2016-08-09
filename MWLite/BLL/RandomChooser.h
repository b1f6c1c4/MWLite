#pragma once
#include "../stdafx.h"
#include "../Entities/BlockSet.h"
#include <random>

// ReSharper disable once CppTemplateParameterShadowing
template <typename T, template <typename T> typename Set>
class RandomChooser
{
public:
    RandomChooser()
    {
        m_Random.seed(std::random_device()());
    }

    NO_COPY(RandomChooser);
    NO_MOVE(RandomChooser);

    T Choose(const Set<T> &set)
    {
        auto cnt = set.Count();
        ASSERT(cnt > 0);

        std::uniform_int_distribution<> dist(0, static_cast<int>(cnt) - 1);

        auto it = set.begin();
        for (auto res = dist(m_Random); res > 0; res--)
            ++it;

        return *it;
    }

private:
    std::mt19937_64 m_Random;
};

typedef RandomChooser<Block, set_t> RandomBlockChooser;
