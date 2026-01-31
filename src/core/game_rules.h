#pragma once
#include <vector>
#include "../model/pile.h"
#include "../model/card.h"

bool isValidRun(const std::vector<Card>& cards, const Pile& pile, int startIndex);
bool canDropRun(const std::vector<Card>& cards, const Pile& destination, int movingCardIndex);
bool canDeal(const std::vector<Pile>& piles, int stockPile, int firstTableau, int tableauCount);