#include "game_rules.h"

bool isValidRun(const std::vector<Card>& cards, const Pile& pile, const int startIndex) {
    const auto& indices = pile.cardIndices;

    for (int i = startIndex; i < indices.size() - 1; i++) {
        const Card& a = cards[indices[i]];
        const Card& b = cards[indices[i + 1]];

        if (!a.faceUp || !b.faceUp) return false;
        if (a.suit != b.suit) return false;
        if (a.rank != b.rank + 1) return false;
    }
    return true;
}

bool canDropRun(const std::vector<Card>& cards, const Pile& destination, const int movingCardIndex) {
    if (destination.type == PileType::Completed || destination.type == PileType::Stock) return false;
    if (destination.cardIndices.empty()) return true;

    const Card& top = cards[destination.cardIndices.back()];
    const Card& moving = cards[movingCardIndex];

    return top.rank == moving.rank + 1;
}

bool canDeal(const std::vector<Pile>& piles, const int stockPile, const int firstTableau, const int tableauCount) {
    if (piles[stockPile].cardIndices.size() < tableauCount) return false;
    for (int i = 0; i < tableauCount; i++) {
        if (piles[firstTableau + i].cardIndices.empty()) return false;
    }
    return true;
}