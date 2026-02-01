#pragma once
#include <vector>
#include "../model/pile.h"
#include "../model/card.h"

bool isValidRun(const std::vector<Card>& cards, const Pile& pile, int startIndex);
int findTopRun(const std::vector<Card>& cards, const Pile& pile);
bool canDropRun(const std::vector<Card>& cards, const Pile& destination, int movingCardIndex);
bool canDeal(const std::vector<Pile>& piles, int stockPile, int firstTableau, int tableauCount);

struct GameRules {
    virtual bool canPickUp(const std::vector<Card>& cards, const Pile& pile, int startIndex) = 0;
    virtual bool canDrop(const std::vector<Card>& cards, const Pile& from, const Pile& to, int movingCardIndex) = 0;
    virtual void onDrop(std::vector<Card>& cards, std::vector<Pile>& piles, int fromPile, int toPile) = 0;
    virtual bool canDeal(const std::vector<Pile>& piles, int stockPile, int firstTableau, int tableauCount) = 0;
    virtual void deal(std::vector<Card>& cards, std::vector<Pile>& piles) = 0;

    virtual ~GameRules() = default;
};