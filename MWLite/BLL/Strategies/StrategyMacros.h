#pragma once

#define B (*state.OpenNoMines)
#define C (*state.ClosedBlocks)
#define M (*state.OpenMines)
#define U(b) (*state.Neighborhood(b))
#define f(b) (state.NeighborCount(b))

#define USE_LOGIC_T(cls) cls cls##_inst
#define USE_LOGIC(cls) (cls##_inst).Update(state, Mt, Bt)

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
