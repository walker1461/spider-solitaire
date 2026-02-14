#include "card.h"
#include <string>
#include <unordered_map>
#include "core/path.h"

inline const char* suitToString(const Suit suit) {
    switch (suit) {
        case Suit::Spades: return "Spades";
        case Suit::Hearts: return "Hearts";
        case Suit::Clubs: return "Clubs";
        case Suit::Diamond: return "Diamond";
    }
    return "";
}

std::filesystem::path getCardTexturePath(const Suit suit, const int rank) {
    return path::assetPath(
              std::filesystem::path("cards") /
              (std::string(suitToString(suit)) + " " + std::to_string(rank) + ".png")
        );
}

std::vector<Card> generateDeck(int numToGenerate) {

   // std::vector<Suit> suits;
    std::vector<Suit> suits{Suit::Spades};

    //if (suitCount == 1) {
    //   suits = {Suit::Spades};
    //} else if (suitCount == 2) {
    //    suits = {Suit::Spades, Suit::Hearts};
    //} else {
    //    suits = {Suit::Spades, Suit::Hearts, Suit::Diamond, Suit::Clubs};
    //}

    //int const decksPerSuit = 104 / (13 * suitCount);
    std::vector<Card> deck;

    //for (int d = 0; d < decksPerSuit; d++) {
    for (int d = 0; d < numToGenerate; d++) {
        for (const Suit suit : suits) {
            for (int rank = 1; rank <= 13; rank++) {
                Card card{};
                card.suit = suit;
                card.rank = rank;
                card.faceUp = false;

                card.size = {0.0f, 0.0f};
                card.scale = 1.0f;
                card.cornerRadius = 0.02f;
                card.isDragging = false;
                card.isLifted = false;
                card.liftGroupId = -1;
                card.indexInPile = -1;
                card.pileIndex = -1;

                std::string const path = getCardTexturePath(suit, rank);
                deck.push_back(card);
            }
        }
    }
    return deck;
}