#include "card_drag.h"
#include <algorithm>
#include <vector>
#include "card.h"

bool isValidMove(const std::vector<Card>& cards, const Pile& destination, const int movingCardIndex) {
    if (destination.cardIndices.empty()) return true;
    const Card& top = cards[destination.cardIndices.back()];
    const Card& moving = cards[movingCardIndex];
    return top.rank == moving.rank + 1;
}

void moveCardToPile(const int draggingIndex, const int toPile, std::vector<Card>& cards, std::vector<Pile>& piles) {
    Card& card = cards[draggingIndex];

    if (!isValidMove(cards, piles[toPile], draggingIndex)) return;

    const int fromPile = card.pileIndex;
    auto& fromVec = piles[fromPile].cardIndices;
    fromVec.erase(std::remove(fromVec.begin(), fromVec.end(), draggingIndex), fromVec.end());

    if (!fromVec.empty()) {
        const int topCardIndex = piles[fromPile].cardIndices.back();
        cards[topCardIndex].faceUp = true;
    }

    auto& toVec = piles[toPile].cardIndices;
    toVec.push_back(draggingIndex);
    card.pileIndex = toPile;
}

void DragController::update(const Vec2& mousePos, const bool mouseDown, std::vector<Card>& cards, std::vector<Pile>& piles) {
    const bool justPressed  = mouseDown && !wasMouseDown;
    const bool justReleased = !mouseDown && wasMouseDown;

    // --------- START DRAG ---------
    if (justPressed && draggingRun.empty()) {
        if (pointInPile(mousePos, piles[10], cards[0].size)) {
            if (canDeal(piles, 10, 0, 10)) {
                restock(cards, piles, 10, 0, 10);
            }
            return;
        }
        int maxIndex = 0;
        for (Card& c : cards) {
            maxIndex = std::max(maxIndex, c.indexInPile);
        }
        for (int p = 0; p < piles.size(); p++) {
            Pile& pile = piles[p];

            for (int i = pile.cardIndices.size() - 1; i >= 0; i--) {
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
                }
                for (int i = 0; i < draggingRun.size(); i++) {
                    cards[draggingRun[i]].indexInPile = maxIndex + i + 1;
                }
                dragOffset = mousePos - card.position;
                return;
            }
        }
    }

    // --------- DRAG CARD ---------
    if (mouseDown && !draggingRun.empty()) {
        constexpr float spacing = 0.07f;
        Vec2 base = mousePos - dragOffset;

        for (int i = 0; i < draggingRun.size(); i++) {
            Card& card = cards[draggingRun[i]];
            card.position.x = base.x;
            card.position.y = base.y - i * spacing;
            card.scale = 1.1f;
        }
    }

    // --------- DROP CARD ---------
    if (justReleased && draggingPile != -1) {
        const int leadCardIndex = draggingRun.front();
        const Card& card = cards[leadCardIndex];
        int targetPile = -1;

        for (int i = 0; i < piles.size(); i++) {
            const Pile& pile = piles[i];
            if (const float halfWidth = card.size.x * 0.5f; card.position.x >= pile.basePosition.x - halfWidth &&
                                                            card.position.x <= pile.basePosition.x + halfWidth) {
                targetPile = i;
                break;
            }
        }
        if (targetPile != -1) {
            moveRun(cards, piles, draggingPile, targetPile, draggingStartIndex);
        }

        for (const int idx : draggingRun) {
            cards[idx].isDragging = false;
            cards[idx].scale = 1.0f;
        }

        draggingRun.clear();
        draggingPile = -1;
        draggingStartIndex = -1;
    }
    wasMouseDown = mouseDown;
}

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
    if (destination.cardIndices.empty()) return true;
    const Card& top = cards[destination.cardIndices.back()];
    const Card& moving = cards[movingCardIndex];
    return top.rank == moving.rank + 1;
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
}