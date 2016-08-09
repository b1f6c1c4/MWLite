#pragma once

#define B (*state.OpenNoMines)
#define C (*state.ClosedBlocks)
#define M (*state.OpenMines)
#define U (state.Neighborhood)
#define f (state.NeighborCount)

#define USE_LOGIC(cls) ((cls()).Update(state, Mt, Bt))

inline void FixPoint(std::function<void()> fun)
{
    fun();
}

template <typename T, typename ... TArgs>
inline void FixPoint(std::function<void()> fun, T arg0, TArgs ... args)
{
    while (true)
    {
        auto old = arg0();

        FixPoint(fun, args...);

        if (old == arg0())
            return;
    }
}

#define REPEAT FixPoint([&]() 

#define UNTIL(arg) ,[&](){ return (arg); }

#define END_REPEAT );
