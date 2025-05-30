#pragma once


#include "world.h"


/// Начальное игровое положение.
struct InitialPosition
{
    Coordinate playerColumn;
    std::array<Coordinate, BOXES_COLUMNS> boxes;
};

const std::vector<InitialPosition> INITIAL_POSITIONS
{
    InitialPosition{ 3, { 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 } },
    InitialPosition{ 4, { 0, 0, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0 } },
    InitialPosition{ 5, { 0, 0, 0, 1, 2, 3, 2, 1, 0, 0, 0, 0 } },
    InitialPosition{ 2, { 0, 3, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5 } },
    InitialPosition{ 1, { 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 2 } },
    InitialPosition{ 1, { 5, 0, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5 } },
    InitialPosition{ 4, { 1, 2, 0, 2, 1, 2, 1, 1, 1, 1, 1, 1 } },
    InitialPosition{ 3, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
};
