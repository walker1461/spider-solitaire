#pragma once
#include "vectors.h"
#include <vector>
#include <string>
#include "glad/glad.h"

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
    float dragOffsetX;
    float dragOffsetY;

    int pileIndex;
    int indexInPile;

    GLuint textureID;
};

enum struct PileType {
    Tableau,
    Stock,
    Completed
};

struct Pile {
    PileType type;
    Vec2 basePosition;
    std::vector<int> cardIndices;
};

struct DealState {
    bool active = false;
    float timer = 0.0f;
    float delay = 0.08f;
    int nextPile = 0;
};

bool canDeal(const std::vector<Pile>& piles, int stockPile, int firstTableau, int tableauCount);
void restock(std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);
void startDealing(DealState& deal);
void updateDealing(DealState& deal, float deltaTime, std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);
bool pointInCard(const Vec2& mouse, const Card& card);
bool pointInPile(const Vec2& mouse, const Pile& pile, const Vec2& cardSize);
void updateCardPositions(std::vector<Card>& cards, const std::vector<Pile>& piles);
inline const char* suitToString(Suit suit);

std::string getCardTexture(Suit suit, int rank);
std::vector<Card> generateDeck(int suitCount);