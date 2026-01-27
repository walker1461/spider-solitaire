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

void restock(std::vector<Card>& cards, std::vector<Pile>& piles, const int stockPile, const int firstTableau, const int tableauCount) {
    for (int i = 0; i < tableauCount; i++) {
        Pile& stock = piles[stockPile];
        Pile& tableau = piles[firstTableau + i];

        int cardIndex = stock.cardIndices.back();
        stock.cardIndices.pop_back();

        Card& card = cards[cardIndex];
        card.faceUp = true;
        card.pileIndex = firstTableau + i;

        tableau.cardIndices.push_back(cardIndex);
    }
}

bool pointInCard(const Vec2 &mouse, const Card &card) {
    const float halfWidth = card.size.x * card.scale * 0.5f;
    const float halfHeight = card.size.y * card.scale * 0.5f;

    return mouse.x >= card.position.x - halfWidth &&
           mouse.x <= card.position.x + halfWidth &&
           mouse.y >= card.position.y - halfHeight &&
           mouse.y <= card.position.y + halfHeight;
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
    for (const auto &pile : piles) {
        if (pile.type != PileType::Stock) {
            for (int i = 0; i < pile.cardIndices.size(); i++) {
                Card& card = cards[pile.cardIndices[i]];
                if (card.isDragging) {
                    continue;
                }
                card.position.x = pile.basePosition.x;
                card.position.y = pile.basePosition.y - i * 0.07f;
                card.indexInPile = i;
            }
        } else {
            for (int i = 0; i < pile.cardIndices.size(); i++) {
                Card& card = cards[pile.cardIndices[i]];
                card.position = pile.basePosition;
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

std::string getCardTexture(Suit suit, int rank) {
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

