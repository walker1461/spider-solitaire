#pragma once
#include "../math/vectors.h"
#include <vector>
#include <string>
#include "glad/glad.h"
#include "dealState.h"

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

    int pileIndex;
    int indexInPile;

    GLuint textureID;
    bool isDark;
};

// void startDealing(DealState& deal);

inline const char* suitToString(Suit suit);

std::string getCardTexture(Suit suit, int rank);
std::vector<Card> generateDeck(int suitCount);