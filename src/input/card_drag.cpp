#include "card_drag.h"
#include "../core/game_rules.h"
#include <algorithm>
#include <vector>
#include "../model/card.h"
#include "../core/layout.h"

Vec2 screenToWorld(const double x, const double y, const int w, const int h) {
    const auto ndcX = static_cast<float>(2.0 * x / w - 1.0);
    const auto ndcY = static_cast<float>(1.0 - 2.0 * y / h);
    return {ndcX, ndcY};
}

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

void autoComplete(std::vector<Card>& cards, std::vector<Pile>& piles, const int fromPile, const std::vector<int> &draggingRun, const int draggingStartIndex) {
    int toPile = -1;
    for (int i = 11; i < piles.size(); i++) {
        if (piles[i].type == PileType::Completed && piles[i].cardIndices.empty()) {
            toPile = i;
            break;
        }
    }
    if (toPile == -1) return; // exit upon winning or error
    Pile& destination = piles[toPile];
    Pile& source = piles[fromPile];

    for (int cardIndex : draggingRun) {
        destination.cardIndices.push_back(cardIndex);
        cards[cardIndex].pileIndex = toPile;
    }

    source.cardIndices.erase(source.cardIndices.begin() + draggingStartIndex, source.cardIndices.end());
    if (!source.cardIndices.empty()) {
        cards[source.cardIndices.back()].faceUp = true;
    }
}

void moveRun(std::vector<Card>& cards, std::vector<Pile>& piles, const int fromPile, const int toPile, const int startIndex) {
    Pile& source = piles[fromPile];
    Pile& destination = piles[toPile];
    const int movingCard = source.cardIndices[startIndex];

    if (!isValidRun(cards, source, startIndex)) return;
    if (!canDropRun(cards, destination, movingCard)) return;

    std::vector<int> run;
    for (int i = startIndex; i < source.cardIndices.size(); i++) {
        run.push_back(source.cardIndices[i]);
    }
    source.cardIndices.erase(source.cardIndices.begin() + startIndex, source.cardIndices.end());

    if (!source.cardIndices.empty()) {
        cards[source.cardIndices.back()].faceUp = true;
    }

    for (int idx : run) {
        destination.cardIndices.push_back(idx);
        cards[idx].pileIndex = toPile;
        cards[idx].isDragging = false;
        cards[idx].scale = 1.0f;
    }

    if (isCompleteRun(cards, destination)) {
        std::vector<int> completeRun;
        for (int i = static_cast<int>(destination.cardIndices.size() -13); i < destination.cardIndices.size(); i++) {
            completeRun.push_back(destination.cardIndices[i]);
        }
        autoComplete(cards, piles, toPile, completeRun, static_cast<int>(destination.cardIndices.size() - 13));
    }
}

void DragController::updateIdle(const Vec2 &mousePos, const bool justPressed, std::vector<Card> &cards, std::vector<Pile> &piles, const Pile& stock, DealState& dealState) {
    if (!justPressed) return;
    if (pointInPile(mousePos, stock, {0.22f, 0.3f}) && canDeal(piles, 10, 0, 10)) {
        dealState.active = true;
        dealState.nextPile = 0;
        dealState.timer = 0.0f;
        return;
    }

    int maxIndex = 0;
    for (Card& card : cards) {
        maxIndex = std::max(maxIndex, card.indexInPile);
    }
    for (int p = 0; p < piles.size() - 8; p++) {
        Pile& pile = piles[p];

        for (int i = static_cast<int>(pile.cardIndices.size() - 1); i >= 0; i--) {
            const int cardIndex = pile.cardIndices[i];
            Card& card = cards[cardIndex];
            if (!card.faceUp) break;
            if (!pointInCard(mousePos, card)) continue;
            if (!isValidRun(cards, pile, i)) continue;

            draggingPile = p;
            draggingStartIndex = i;

            for (int j = i; j < pile.cardIndices.size(); j++) {
                int idx = pile.cardIndices[j];
                draggingRun.push_back(idx);
                cards[idx].isDragging = true;
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

        for (int i = 0; i < piles.size() - 8; i++) {
            const Pile& pile = piles[i];
            float halfWidth = card.size.x * 0.5f;
            if (card.targetPosition.x >= pile.basePosition.x - halfWidth &&
                card.targetPosition.x <= pile.basePosition.x + halfWidth) {
                targetPile = i;
                break;
            }
        }

        if (targetPile != -1) {
            moveRun(cards, piles, draggingPile, targetPile, draggingStartIndex);
        }

        for (const int idx : draggingRun) {
            Card& c = cards[idx];
            c.isDragging = false;
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

void DragController::update(const Vec2& mousePos, bool mouseDown,
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