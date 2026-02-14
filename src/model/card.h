#pragma once
#include <filesystem>
#include <vector>
#include "../math/vectors.h"

enum class Suit {
    Spades,
    Hearts,
    Diamond,
    Clubs
};

struct Card {
    int rank;
    Suit suit;
    bool faceUp;

    Vec2 targetPosition;
    Vec2 visualPosition;

    Vec2 size;
    float scale;
    float cornerRadius;

    bool isDragging;
    bool isLifted;
    int liftGroupId;

    int pileIndex;
    int indexInPile;

    Vec2 velocity{0.0f, 0.0f};
    float rotation = 0.0f;
    float alpha = 1.0f;
    bool isFlying = false;

    bool isActive = true;
    bool isDark;
};

inline const char* suitToString(Suit suit);

std::filesystem::path getCardTexturePath(Suit suit, int rank);
std::vector<Card> generateDeck(int numToGenerate);