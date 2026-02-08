#pragma once
#include <vector>
#include "game_rules.h"
#include "../model/autoState.h"

class SpiderRules : public GameRules {
public:
    [[nodiscard]] GameConfig config() const override;

    bool canPickUp(const std::vector<Card> &cards, const Pile &pile, int startIndex) override;
    bool canDrop(const std::vector<Card> &cards, const Pile &from, const Pile &to, int movingCardIndex) override;
    void onDrop(std::vector<Card> &cards, std::vector<Pile> &piles, int fromPile, int toPile, int startIndex, AutoState &autoState, float deltaTime) override;

    bool canDeal(const std::vector<Pile> &piles, int stockPile, int firstTableau, int tableauCount) override;
    void deal(std::vector<Card> &cards, std::vector<Pile> &piles) override;

    bool checkForWin(std::vector<Pile> &piles) override;

    void darkenCards(std::vector<Card>& cards, std::vector<Pile>& piles) override;
};
