#include "card.h"
#include <filesystem>
#include "render.h"
#include <string>
#include <unordered_map>

static std::unordered_map<std::string, GLuint> textureCache;

bool canDeal(const std::vector<Pile>& piles, const int stockPile, const int firstTableau, const int tableauCount) {
    if (piles[stockPile].cardIndices.size() < tableauCount) return false;
    for (int i = 0; i < tableauCount; i++) {
        if (piles[firstTableau + i].cardIndices.empty()) return false;
    }
    return true;
}

void restock(std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock) {
    if (stock.cardIndices.empty()) return;
    constexpr int NUM_TABLEAU = 10;

    for (int i = 0; i < NUM_TABLEAU; i++) {
        if (stock.cardIndices.empty()) return;
        int cardIndex = stock.cardIndices.back();
        stock.cardIndices.pop_back();

        Card& card = cards[cardIndex];
        card.pileIndex = i;
        card.indexInPile = piles[i].cardIndices.size();
        card.faceUp = true;
        card.visualPosition = piles[i].basePosition;
        piles[i].cardIndices.push_back(cardIndex);
    }
}

void startDealing(DealState& deal) {
    deal.active = true;
    deal.timer = 0.0f;
    deal.nextPile = 0;
}

void updateDealing(DealState& deal, const float deltaTime, std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock) {
    if (!deal.active) return;
    deal.timer += deltaTime;
    if (deal.timer < deal.delay) return;
    deal.timer = 0.0f;
    if (stock.cardIndices.empty() || deal.nextPile >= 10) {
        deal.active = false;
        return;
    }
    const int cardIndex = stock.cardIndices.back();
    stock.cardIndices.pop_back();
    Card& card = cards[cardIndex];
    card.faceUp = true;
    card.pileIndex = deal.nextPile;
    card.indexInPile = piles[deal.nextPile].cardIndices.size();

    card.visualPosition = stock.basePosition;
    card.targetPosition = piles[deal.nextPile].basePosition;

    piles[deal.nextPile].cardIndices.push_back(cardIndex);
    deal.nextPile++;
}

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

void updateCardPositions(std::vector<Card>& cards, const std::vector<Pile>& piles) {
    float maxHeight = 1.8f;
    float baseSpacing = 0.07f;
    float spacing = baseSpacing;

    for (const auto &pile : piles) {
        if (pile.cardIndices.size() * spacing > maxHeight) {
            spacing = maxHeight / pile.cardIndices.size();
        }
        if (pile.type != PileType::Stock && pile.type != PileType::Completed) {
            for (int i = 0; i < pile.cardIndices.size(); i++) {
                Card& card = cards[pile.cardIndices[i]];
                if (card.isDragging) {
                    continue;
                }
                card.targetPosition.x = pile.basePosition.x;
                card.targetPosition.y = pile.basePosition.y - i * 0.07f;
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

inline const char* suitToString(const Suit suit) {
    switch (suit) {
        case Suit::Spades: return "Spades";
        case Suit::Hearts: return "Hearts";
        case Suit::Clubs: return "Clubs";
        case Suit::Diamond: return "Diamond";
    }
    return "";
}

std::string getCardTexture(const Suit suit, const int rank) {
    return "textures/" +
           std::string(suitToString(suit)) +
           " " +
           std::to_string(rank) +
           ".png";
}

std::vector<Card> generateDeck(int suitCount) {
    std::vector<Suit> suits;

    if (suitCount == 1) {
        suits = {Suit::Spades};
    } else if (suitCount == 2) {
        suits = {Suit::Spades, Suit::Hearts};
    } else {
        suits = {Suit::Spades, Suit::Hearts, Suit::Diamond, Suit::Clubs};
    }

    int decksPerSuit = 104 / (13 * suitCount);
    std::vector<Card> deck;

    for (int d = 0; d < decksPerSuit; d++) {
        for (Suit suit : suits) {
            for (int rank = 1; rank <= 13; rank++) {
                Card card{};
                card.suit = suit;
                card.rank = rank;
                card.faceUp = false;

                card.size = {0.0f, 0.0f};
                card.scale = 1.0f;
                card.cornerRadius = 0.02f;
                card.isDragging = false;
                card.indexInPile = -1;
                card.pileIndex = -1;

                std::string path = getCardTexture(suit, rank);
                if (textureCache.find(path) == textureCache.end()) {
                    textureCache[path] = loadTexture(path.c_str());
                }
                card.textureID = textureCache[path];
                deck.push_back(card);
            }
        }
    }
    return deck;
}