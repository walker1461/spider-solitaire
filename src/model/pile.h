#pragma once
#include <vector>
#include "../math/vectors.h"

enum struct PileType {
    Tableau,
    Stock,
    Completed
};

struct Pile {
    PileType type;
    Vec2 basePosition;
    std::vector<int> cardIndices;
    int maxCards = -1;
    bool fanDown = true;
};