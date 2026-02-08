#include "spider_rules.h"
#include "run_rules.h"
#include "../model/pile.h"
#include "../core/move_system.h"
#include "../model/autoState.h"

namespace {
    constexpr GameConfig kSpiderConfig{
        .firstTableauPileIndex = 0,
        .tableauCount = 10,
        .stockPileIndex = 10,
        .firstCompletedPileIndex = 11,
        .completedPileCount = 8,
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

    void autoComplete(std::vector<Card>& cards, std::vector<Pile>& piles, const GameConfig& cfg, const int fromPile,
                      const std::vector<int>& draggingRun, const int draggingStartIndex, AutoState& autoState) {
        int toPile = -1;

        const int start = cfg.firstCompletedPileIndex;
        const int end = start + cfg.completedPileCount;

        for (int i = start; i < end; i++) {
            if (piles[i].type == PileType::Completed && piles[i].cardIndices.empty()) {
                toPile = i;
                break;
            }
        }

        if (toPile == -1) return;

        autoState.active = true;
        autoState.timer = 0.0f;
        autoState.fromPile = fromPile;
        autoState.toPile = toPile;
        autoState.cardIndices = draggingRun;

        Pile& source = piles[fromPile];
        source.cardIndices.erase(source.cardIndices.begin() + draggingStartIndex, source.cardIndices.end());
        if (!source.cardIndices.empty()) {
            cards[source.cardIndices.back()].faceUp = true;
        }

        static int nextAutoLiftGroupId = 200000;
        int const groupId = nextAutoLiftGroupId++;

        for (const int cardIndex : draggingRun) {
            cards[cardIndex].isLifted = true;
            cards[cardIndex].liftGroupId = groupId;
        }
    }
}

GameConfig SpiderRules::config() const {
    return kSpiderConfig;
}

void SpiderRules::onDrop(std::vector<Card> &cards, std::vector<Pile> &piles, const int fromPile,
                         const int toPile, const int startIndex, AutoState& autoState, float deltaTime) {
    moveRun(cards, piles, fromPile, toPile, startIndex);

    if (const Pile& destination = piles[toPile]; isCompleteRun(cards, destination)) {
        std::vector<int> completeRun;
        for (int i = static_cast<int>(destination.cardIndices.size() - 1); i >= static_cast<int>(destination.cardIndices.size() - 13); i--) {
            completeRun.push_back(destination.cardIndices[i]);
        }
        autoComplete(cards, piles, kSpiderConfig, toPile, completeRun, static_cast<int>(destination.cardIndices.size() - 13), autoState);
    }
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

    for (int i = 0; i < tableauCount; i++) {
        if (piles[firstTableau + i].cardIndices.empty()) return false;
    }

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

bool SpiderRules::checkForWin(std::vector<Pile> &piles) {
    for (int i = kSpiderConfig.firstCompletedPileIndex; i < kSpiderConfig.firstCompletedPileIndex + kSpiderConfig.completedPileCount; i++) {
        if (piles[i].cardIndices.empty()) return false;
    }
    return true;
}

void SpiderRules::darkenCards(std::vector<Card>& cards, std::vector<Pile>& piles) {
    for (Pile& pile : piles) {
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