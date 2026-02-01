#include "layout.h"

constexpr float TABLEAU_BOTTOM = -0.9f;
constexpr float VIRTUAL_WIDTH = 2.0f;
constexpr float VIRTUAL_HEIGHT = 2.0f;

void updateCardPositions(std::vector<Card>& cards, const std::vector<Pile>& piles) {
    for (const auto &pile : piles) {

        float maxHeight = 1.2f;
        constexpr float baseSpacing = 0.07f;
        float spacing = baseSpacing;

        if (static_cast<float>(pile.cardIndices.size()) * spacing > maxHeight) {
            spacing = maxHeight / static_cast<float>(pile.cardIndices.size());
        }
        if (pile.type != PileType::Stock && pile.type != PileType::Completed) {
            for (int i = 0; i < pile.cardIndices.size(); i++) {
                Card& card = cards[pile.cardIndices[i]];
                if (card.isDragging) {
                    continue;
                }
                card.targetPosition.x = pile.basePosition.x;
                card.targetPosition.y = pile.basePosition.y - static_cast<float>(i) * spacing;
                card.indexInPile = i;
            }
        } else {
            for (int i = 0; i < pile.cardIndices.size(); i++) {
                Card& card = cards[pile.cardIndices[i]];
                card.targetPosition = pile.basePosition;
                card.indexInPile = i;
            }
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
    const float halfW = cardSize.x * 0.5f;
    const float halfH = cardSize.y * 0.5f;

    return mouse.x >= pile.basePosition.x - halfW &&
           mouse.x <= pile.basePosition.x + halfW &&
           mouse.y >= pile.basePosition.y - halfH &&
           mouse.y <= pile.basePosition.y + halfH;
}

void layoutPiles(std::vector<Pile> &piles, const GameConfig& cfg) {
    constexpr float marginX = 0.08f;
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
    piles[cfg.stockPileIndex].basePosition = { 0.9f, topY };

    // completed piles
    for (int i = 0; i < cfg.completedPileCount; i++) {
        piles[cfg.firstCompletedPileIndex + i].basePosition = {
            -1.0f + marginX + spacing * static_cast<float>(i),
            topY
        };
    }
}