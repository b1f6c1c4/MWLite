#pragma once

#define B (*state.OpenNoMines)
#define C (*state.ClosedBlocks)
#define M (*state.OpenMines)
#define U (state.Neighborhood)
#define f (state.NeighborCount)

#define USE_LOGIC(cls) ((cls()).Update(state, Mt, Bt))
