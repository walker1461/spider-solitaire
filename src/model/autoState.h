#pragma once

#include <vector>

struct AutoState {
    bool active = false;
    float timer = 0.0f;
    float delay = 0.08f;

    int fromPile = -1;
    int toPile = -1;
    std::vector<int> cardIndices;
};