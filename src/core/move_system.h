#pragma once
#include <vector>

#include "../model/card.h"
#include "../model/pile.h"

void moveRun(
    std::vector<Card>& cards, std::vector<Pile>& piles,
    int fromPile, int toPile, int startIndex
);
