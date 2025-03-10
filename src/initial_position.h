#pragma once


#include "world.h"


/// Начальное игровое положение
struct InitialPosition
{
    unsigned char playerColumn;
    std::array<unsigned char, BOXES_COLUMNS> boxes;
};

const std::vector<InitialPosition> INITIAL_POSITIONS
{
    InitialPosition{ 4, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 } },
    InitialPosition{ 4, { 0, 0, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0 } }
};
