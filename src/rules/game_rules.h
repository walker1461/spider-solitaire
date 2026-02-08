#pragma once
#include <vector>
#include "../model/pile.h"
#include "../model/card.h"
#include "../core/game_config.h"

struct AutoState;

struct GameRules {
    [[nodiscard]] virtual GameConfig config() const = 0;

    virtual bool canPickUp(const std::vector<Card>& cards, const Pile& pile, int startIndex) = 0;
    virtual bool canDrop(const std::vector<Card>& cards, const Pile& from, const Pile& to, int movingCardIndex) = 0;
    virtual void onDrop(std::vector<Card>& cards, std::vector<Pile>& piles, int fromPile, int toPile, int startIndex, AutoState &autoState, float deltaTime) = 0;

    virtual bool canDeal(const std::vector<Pile>& piles, int stockPile, int firstTableau, int tableauCount) = 0;
    virtual void deal(std::vector<Card>& cards, std::vector<Pile>& piles) = 0;

    virtual void darkenCards(std::vector<Card>& cards, std::vector<Pile>& piles) = 0;

    virtual bool checkForWin(std::vector<Pile>& piles) = 0;

    virtual ~GameRules() = default;
};