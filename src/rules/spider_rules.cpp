#include "spider_rules.h"

#include <cmath>
#include <iostream>

#include "run_rules.h"
#include "../model/pile.h"
#include "../core/move_system.h"

namespace {
    constexpr GameConfig kSpiderConfig{
        .firstTableauPileIndex = 0,
        .tableauCount = 10,
        .stockPileIndex = 10,
        .firstCompletedPileIndex = 11,
        .completedPileCount = 8,
        .maxPileSize = -1,
        .cardSize = {0.22f, 0.3f},
    };

    bool isCompleteRun(const std::vector<Card>& cards, const Pile& pile) {
        if (pile.cardIndices.size() < 13) return false;
        const int start = static_cast<int>(pile.cardIndices.size() - 13);

        for (int i = start; i < pile.cardIndices.size() - 1; i++) {
            const Card& a = cards[pile.cardIndices[i]];
            const Card& b = cards[pile.cardIndices[i + 1]];

            if (!a.faceUp || !b.faceUp) return false;
            if (a.suit != b.suit) return false;
            if (a.rank != b.rank + 1) return false;
        }
        return true;
    }

    void startRunClearAnimation(std::vector<Card>& cards, const std::vector<int>& completeRun,
        std::vector<Pile>& piles, const int toPile)
    {
        Pile& source = piles[toPile];

        // remove the run from the pile
        int runSize = static_cast<int>(completeRun.size());

        if (source.cardIndices.size() >= runSize) {
            source.cardIndices.erase(
                source.cardIndices.end() - runSize,
                source.cardIndices.end()
            );
        }
        else {
            std::cout << "ERROR: run larger than pile\n";
            return;
        }

        // reindex remaining cards in pile
        for (int i = 0; i < source.cardIndices.size(); i++) {
            cards[source.cardIndices[i]].indexInPile = i;
        }

        // flip the new top card face up if needed
        if (!source.cardIndices.empty()) {
            cards[source.cardIndices.back()].faceUp = true;
        }

        // start animation on cleared cards
        for (const int cardIndex : completeRun) {
            Card& card = cards[cardIndex];

            card.isFlying = true;
            card.alpha = 1.0f;
            card.flyTimer = 0.0f;
            card.flyDuration = 1.5f;
            card.flyStart = card.visualPosition;

            card.velocity = {
                ((rand() % 1000) / 1000.0f - 0.5f) * 2.5f,
                1.0f + ((rand() % 1000) / 1000.0f)
            };

            const float angle = ((rand() % 1000) / 1000.0f) * 6.28318f; // 0–2π
            const float speed = 1.5f + ((rand() % 1000) / 1000.0f) * 1.5f;

            const Vec2 dir = { cos(angle), sin(angle) };

            card.flyTarget = {
                card.visualPosition.x + dir.x * speed,
                card.visualPosition.y + dir.y * speed
            };

        }
    }
}

GameConfig SpiderRules::config() const {
    return kSpiderConfig;
}

DropResult SpiderRules::onDrop(std::vector<Card> &cards, std::vector<Pile> &piles, const int fromPile,
                         const int toPile, const int startIndex) {
    moveRun(cards, piles, fromPile, toPile, startIndex);

    DropResult result;

    if (const Pile& destination = piles[toPile]; isCompleteRun(cards, destination)) {
        std::vector<int> completeRun;
        for (int i = static_cast<int>(destination.cardIndices.size() - 1); i >= static_cast<int>(destination.cardIndices.size() - 13); i--) {
            completeRun.push_back(destination.cardIndices[i]);
        }
        startRunClearAnimation(cards, completeRun, piles, toPile);
        result.runCleared = true;
        result.runsCleared = 1;
    }
    return result;
}

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
                          const int movingCardIndex) {

    return canDropRun(cards, to, movingCardIndex);
}

bool SpiderRules::canDeal(const std::vector<Pile> &piles, const int stockPile,
                          const int firstTableau, const int tableauCount) {

    if (piles[stockPile].cardIndices.size() < tableauCount) return false;

    // for (int i = 0; i < tableauCount; i++) {
    //     if (piles[firstTableau + i].cardIndices.empty()) return false;
    // }

    return true;
}

void SpiderRules::deal(std::vector<Card> &cards, std::vector<Pile> &piles){
    constexpr int stock = kSpiderConfig.stockPileIndex;
    constexpr int firstTableau = kSpiderConfig.firstTableauPileIndex;
    constexpr int tableauCount = kSpiderConfig.tableauCount;

    for (int i = 0; i < tableauCount; i++) {
        int const card = piles[stock].cardIndices.back();
        piles[stock].cardIndices.pop_back();

        piles[firstTableau + i].cardIndices.push_back(card);
        cards[card].faceUp = true;
    }
}

void SpiderRules::checkPileSizes(std::vector<Pile> &piles) {
    for (int i = 0; i < kSpiderConfig.tableauCount; i++) {
        if (piles[i].cardIndices.size() > kSpiderConfig.maxPileSize) {
            piles[i].tooTall = true;
        } else {
            piles[i].tooTall = false;
        }
    }
}

bool SpiderRules::checkForGameOver(std::vector<Pile> &piles, int maxPileSize) {
    for (const Pile &pile : piles) {
        if (pile.cardIndices.size() > maxPileSize + 1 && pile.type == PileType::Tableau) {
            return true;
        }
    }
    return false;
}

void SpiderRules::darkenCards(std::vector<Card> &cards, std::vector<Pile> &piles) {
    for (Pile &pile : piles) {
        std::vector<int> const runToCheck;
        // completed piles and stock should not be darkened
        if (pile.type == PileType::Completed || pile.type == PileType::Stock) {
            for (const int cardIndex : pile.cardIndices) {
                cards[cardIndex].isDark = false;
            }
            continue;
        }

        const int start = findTopRun(cards, pile);

        // cards in run at the top of a tableau pile will be bright
        for (int i = 0; i < pile.cardIndices.size(); i++) {
            const int cardIndex = pile.cardIndices[i];
            cards[cardIndex].isDark = (start == -1 || i < start);
        }
    }
}