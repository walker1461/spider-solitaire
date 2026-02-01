#pragma once
#include "../math/vectors.h"

struct GameConfig {
    int tableauCount{};
    int stockPileIndex{};
    int firstCompletedPileIndex{};
    Vec2 cardSize{0.22f, 0.3f};
};