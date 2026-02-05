#include "card_drag.h"
#include "../rules/game_rules.h"
#include <algorithm>
#include <vector>
#include "../model/card.h"
#include "../core/layout.h"

Vec2 screenToWorld(const double x, const double y, const int w, const int h) {
    const auto ndcX = static_cast<float>(2.0 * x / w - 1.0);
    const auto ndcY = static_cast<float>(1.0 - 2.0 * y / h);
    return {ndcX, ndcY};
}

void DragController::setRules(GameRules *rules) {
    gameRules = rules;
}

void DragController::setConfig(const GameConfig *config) {
    gameConfig = config;
}

void DragController::updateIdle(const Vec2 &mousePos, const bool justPressed, std::vector<Card> &cards,
                                std::vector<Pile> &piles, const Pile& stock, DealState& dealState) {
    if (!justPressed || gameRules == nullptr || gameConfig == nullptr) return;
    if (pointInPile(mousePos, stock, gameConfig->cardSize) && gameRules->canDeal(piles, gameConfig->stockPileIndex,
          0, gameConfig->tableauCount)) {
        dealState.active = true;
        dealState.nextPile = 0;
        dealState.timer = 0.0f;
        return;
    }

    int maxIndex = 0;
    for (Card& card : cards) {
        maxIndex = std::max(maxIndex, card.indexInPile);
    }
    for (int p = 0; p < gameConfig->tableauCount; p++) {
        Pile& pile = piles[p];

        for (int i = static_cast<int>(pile.cardIndices.size() - 1); i >= 0; i--) {
            const int cardIndex = pile.cardIndices[i];
            Card const& card = cards[cardIndex];
            if (!card.faceUp) break;
            if (!pointInCard(mousePos, card)) continue;
            if (!gameRules->canPickUp(cards, pile, i)) continue;

            draggingPile = p;
            draggingStartIndex = i;

            static int nextLiftGroupId = 1;
            const int groupId = nextLiftGroupId++;

            for (int j = i; j < pile.cardIndices.size(); j++) {
                int const idx = pile.cardIndices[j];
                draggingRun.push_back(idx);
                cards[idx].isDragging = true;
                cards[idx].isLifted = true;
                cards[idx].liftGroupId = groupId;
                cards[idx].indexInPile = maxIndex + (j - i) + 1;
            }

            dragOffset = mousePos - card.visualPosition;
            state = DragState::Dragging;
            return;
        }
    }
}

void DragController::updateDragging(const Vec2 &mousePos, const bool mouseDown, const bool justReleased, std::vector<Card> &cards, std::vector<Pile> &piles, Pile &stock) {
    if (mouseDown) {
        auto [x, y] = mousePos - dragOffset;
        for (int i = 0; i < draggingRun.size(); i++) {
            constexpr float spacing = 0.07f;
            Card& card = cards[draggingRun[i]];

            card.visualPosition.x = x;
            card.visualPosition.y = y - static_cast<float>(i) * (spacing - 0.01f);
            card.targetPosition.x = x;
            card.targetPosition.y = y - static_cast<float>(i) * spacing;
            card.scale = 1.1f;
        }
    }

    if (justReleased) {
        const int leadCardIndex = draggingRun.front();
        const Card& card = cards[leadCardIndex];
        int targetPile = -1;

        if (gameConfig != nullptr) {
            for (int i = 0; i < gameConfig->tableauCount; i++) {
                const Pile& pile = piles[i];
                float const halfWidth = card.size.x * 0.5f;
                if (card.targetPosition.x >= pile.basePosition.x - halfWidth &&
                    card.targetPosition.x <= pile.basePosition.x + halfWidth) {
                    targetPile = i;
                    break;
                }
            }
        }

        if (targetPile != -1 && gameRules != nullptr) {
            const int movingCardIndex = piles[draggingPile].cardIndices[draggingStartIndex];
            if (gameRules->canDrop(cards, piles[draggingPile], piles[targetPile], movingCardIndex)) {
                gameRules->onDrop(cards, piles, draggingPile, targetPile, draggingStartIndex);
            }
        }

        for (const int idx : draggingRun) {
            Card& c = cards[idx];
            c.isDragging = false;
            c.isLifted = true;
            c.scale = 1.0f;
        }

        draggingRun.clear();
        draggingPile = -1;
        draggingStartIndex = -1;
        state = DragState::Idle;
    }
}

void DragController::updateDealing(std::vector<Card> &cards, std::vector<Pile> &piles, Pile &stock) {
    state = DragState::Idle;
}

void DragController::update(const Vec2& mousePos, const bool mouseDown,
                            std::vector<Card>& cards, std::vector<Pile>& piles,
                            Pile& stock, DealState& dealState) {
    if (dealState.active) {
        wasMouseDown = mouseDown;
        return;
    }
    const bool justPressed = mouseDown && !wasMouseDown;
    const bool justReleased = !mouseDown && wasMouseDown;

    switch (state) {
        case DragState::Idle:
            updateIdle(mousePos, justPressed, cards, piles, stock, dealState);
            break;
        case DragState::Dragging:
            updateDragging(mousePos, mouseDown, justReleased, cards, piles, stock);
            break;
    }
    wasMouseDown = mouseDown;
}