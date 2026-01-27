#pragma once
#include <vector>
#include "card.h"
#include "math.h"

bool isValidMove(const std::vector<Card>& cards, const Pile& destination, const int movingCardIndex);
void moveCardToPile(int draggingIndex, int toPile, std::vector<Card>& cards, std::vector<Pile>& piles);
bool isValidRun(const std::vector<Card>& cards, const Pile& pile, const int startIndex);
bool canDropRun(const std::vector<Card>& cards, const Pile& destination, const int movingCardIndex);
void moveRun(std::vector<Card>& cards, std::vector<Pile>& piles, const int fromPile, const int toPile, const int startIndex);

struct DragController {
    int draggingPile = -1;
    int draggingStartIndex = -1;
    std::vector<int> draggingRun;
    Vec2 dragOffset = {0.0f, 0.0f};
    bool wasMouseDown = false;
    void update(const Vec2& mousePos, bool mouseDown, std::vector<Card>& cards, std::vector<Pile>& piles);
};