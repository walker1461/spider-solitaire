#pragma once
#include "../math/vectors.h"

struct GameConfig {
    int firstTableauPileIndex{0};
    int tableauCount{};
    int stockPileIndex{};
    int firstCompletedPileIndex{};
    int completedPileCount{};
    int maxPileSize{};
    Vec2 cardSize{0.22f, 0.3f};
};