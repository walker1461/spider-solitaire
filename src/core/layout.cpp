#include "layout.h"

constexpr float TABLEAU_BOTTOM = -0.9f;
constexpr float VIRTUAL_WIDTH = 2.0f;
constexpr float VIRTUAL_HEIGHT = 2.0f;

void updateCardPositions(std::vector<Card>& cards, std::vector<Pile>& piles) {
    for (int p = 0; p < static_cast<int>(piles.size()); p++) {
        Pile& pile = piles[p];

        constexpr float baseSpacing = 0.07f;
        float spacing = baseSpacing;
        constexpr float faceDownBaseSpacing = 0.04f;
        float faceDownSpacing = faceDownBaseSpacing;
        constexpr float maxHeight = 1.25f;

        int cardCount = static_cast<int>(pile.cardIndices.size());

        if (cardCount * spacing > maxHeight && cardCount > 0) {
            spacing = maxHeight / static_cast<float>(cardCount);
            //faceDownSpacing = (maxHeight / static_cast<float>(pile.cardIndices.size())) - 0.03f;
            faceDownSpacing = spacing - 0.03f;
        }

        //get last face down card
        int lastFaceDownIndex = -1;
        for (int i = 0; i < cardCount; i++) {
            int idx = pile.cardIndices[i];
            if (!cards[idx].faceUp) {
                lastFaceDownIndex = i;
            } else {
                break;
            }
        }

        float const faceDownOffset = (static_cast<float>(lastFaceDownIndex) + 1) * (spacing - faceDownSpacing);
        const bool isTableau = (pile.type != PileType::Stock && pile.type != PileType::Completed);
        float bottomY = pile.basePosition.y;

        for (int i = 0; i < cardCount; ++i) {
            int cardIndex = pile.cardIndices[i];
            if (cardIndex < 0 || cardIndex >= cards.size()) continue;

            Card& card = cards[cardIndex];

            // re-index each card for accuracy
            card.pileIndex = p;
            card.indexInPile = i;

            // don't try to index a card if it's currently dragging
            if (card.isDragging) continue;

            if (isTableau) {
                card.targetPosition.x = pile.basePosition.x;

                if (!card.faceUp) {
                    card.targetPosition.y = pile.basePosition.y - (static_cast<float>(i) * faceDownSpacing);
                } else {
                    card.targetPosition.y = pile.basePosition.y - (static_cast<float>(i) * spacing) + faceDownOffset;
                }

                if (i == cardCount - 1) {
                    bottomY = card.targetPosition.y;
                }

            } else {
                card.targetPosition = pile.basePosition;
            }
        }

        if (isTableau && cardCount > 1) {
            float cardHeight = 0.3f;
            pile.visualHeight = (pile.basePosition.y - bottomY) + cardHeight;
        } else {
            pile.visualHeight = 0.3f;
        }
    }
};

bool pointInCard(const Vec2 &mouse, const Card &card) {
    const float halfWidth = card.size.x * card.scale * 0.5f;
    const float halfHeight = card.size.y * card.scale * 0.5f;

    return mouse.x >= card.targetPosition.x - halfWidth &&
           mouse.x <= card.targetPosition.x + halfWidth &&
           mouse.y >= card.targetPosition.y - halfHeight &&
           mouse.y <= card.targetPosition.y + halfHeight;
};

bool pointInPile(const Vec2& mouse, const Pile& pile, const Vec2& cardSize) {
    const float halfWidth = cardSize.x * 0.5f;
    const float halfHeight = cardSize.y * 0.5f;

    return mouse.x >= pile.basePosition.x - halfWidth &&
           mouse.x <= pile.basePosition.x + halfWidth &&
           mouse.y >= pile.basePosition.y - halfHeight &&
           mouse.y <= pile.basePosition.y + halfHeight;
}

void layoutPiles(std::vector<Pile> &piles, const GameConfig& cfg) {
    constexpr float marginX = 0.14f;
    constexpr float topY = 0.75f;

    constexpr float usableWidth = 2.0f - marginX * 2.0f;
    constexpr float spacing = usableWidth / 9.0f;

    // tableaus
    for (int i = 0; i < cfg.tableauCount; i++) {
        constexpr float tableauY = 0.35f;
        piles[i].basePosition = {
            -1.0f + marginX + spacing * static_cast<float>(i),
            tableauY
        };
    }

    // stock
    piles[cfg.stockPileIndex].basePosition = { 1.0f - marginX, topY };

    // completed piles
    for (int i = 0; i < cfg.completedPileCount; i++) {
        piles[cfg.firstCompletedPileIndex + i].basePosition = {
            -1.0f + marginX + spacing * static_cast<float>(i), topY
        };
    }
}