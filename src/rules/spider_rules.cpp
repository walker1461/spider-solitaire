#include "../games/spider_rules.h"
#include "../core/game_rules.h"
#include "../model/pile.h"
#include "../core/game.h"

// ------ HELPERS ------

bool canDropRun(const std::vector<Card>& cards, const Pile& destination, const int movingCardIndex) {
    if (destination.type == PileType::Completed || destination.type == PileType::Stock) return false;
    if (destination.cardIndices.empty()) return true;

    const Card& top = cards[destination.cardIndices.back()];
    const Card& moving = cards[movingCardIndex];

    return top.rank == moving.rank + 1;
}

bool SpiderRules::canPickUp(const std::vector<Card> &cards, const Pile &pile, const int startIndex) {
    return isValidRun(cards, pile, startIndex);
}

bool SpiderRules::canDrop(const std::vector<Card> &cards, const Pile &from, const Pile &to,
                          const int startIndex) {

    return canDropRun(cards, to, startIndex);
}

void SpiderRules::onDrop(std::vector<Card> &cards, std::vector<Pile> &piles, int fromPile,
                         int toPile, int startIndex) {
    moveRun(cards, piles, fromPile, toPile, startIndex);
}

bool SpiderRules::canDeal(const std::vector<Pile> &piles, const int stockPile,
             const int firstTableau, const int tableauCount) {

    if (piles[stockPile].cardIndices.size() < tableauCount) return false;

    for (int i = 0; i < tableauCount; i++) {
        if (piles[firstTableau + i].cardIndices.empty()) return false;
    }

    return true;
}

void SpiderRules::deal(std::vector<Card> &cards, std::vector<Pile> &piles){
    int stock = 10;
    int firstTableau = 0;
    int tableauCount = 10;

    for (int i = 0; i < tableauCount; i++) {
        int card = piles[stock].cardIndices.back();
        piles[stock].cardIndices.pop_back();

        piles[firstTableau + i].cardIndices.push_back(card);
        cards[card].faceUp = true;
    }
}

std::vector<Pile> spiderLayout = {{PileType::Tableau, {-0.9, 0.4f}, {}, -1, true}};