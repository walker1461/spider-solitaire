#pragma once
#include "../model/card.h"
#include "../model/pile.h"

bool isValidRun(const std::vector<Card>& cards, const Pile& pile, int startIndex);
int findTopRun(const std::vector<Card>& cards, const Pile& pile);
bool canDropRun(const std::vector<Card>& cards, const Pile& destination, int movingCardIndex);