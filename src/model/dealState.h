#pragma once

struct DealState {
    bool active = false;
    float timer = 0.0f;
    float delay = 0.08f;
    int nextPile = 0;
};