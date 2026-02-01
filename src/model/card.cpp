#include "card.h"
#include <filesystem>
#include "../render/render.h"
#include <string>
#include <unordered_map>

static std::unordered_map<std::string, GLuint> textureCache;

// void startDealing(DealState& deal) {
//     deal.active = true;
//     deal.timer = 0.0f;
//     deal.nextPile = 0;
// }

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