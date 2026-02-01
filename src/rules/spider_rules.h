#pragma once
#include <vector>
#include "../core/game_rules.h"

class SpiderRules : public GameRules {
public:
    bool canPickUp(const std::vector<Card> &cards, const Pile &pile, int startIndex) override;
    bool canDrop(const std::vector<Card> &cards, const Pile &from, const Pile &to, int startIndex) override;
    void onDrop(std::vector<Card> &cards, std::vector<Pile> &piles, int fromPile, int toPile, int startIndex) override;
    bool canDeal(const std::vector<Pile> &piles, int stockPile, int firstTableau, int tableauCount) override;
    void deal(std::vector<Card> &cards, std::vector<Pile> &piles) override;
};