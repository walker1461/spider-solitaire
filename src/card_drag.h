#pragma once
#include <vector>
#include "card.h"
#include "vectors.h"

bool isValidRun(const std::vector<Card>& cards, const Pile& pile, int startIndex);
bool canDropRun(const std::vector<Card>& cards, const Pile& destination, int movingCardIndex);
void moveRun(std::vector<Card>& cards, std::vector<Pile>& piles, int fromPile, int toPile, int startIndex);

enum class InputState {
    Idle,
    Dragging
};

class DragController {
    public:
        InputState state = InputState::Idle;

        std::vector<int> draggingRun;
        int draggingPile = -1;
        int draggingStartIndex = -1;
        Vec2 dragOffset;
        bool wasMouseDown = false;

        void update(const Vec2& mousePos, bool mouseDown, std::vector<Card>& cards,
                    std::vector<Pile>& piles, Pile& stock, DealState& dealState);
    private:
        void updateIdle(const Vec2& mousePos, bool justPressed, std::vector<Card>& cards,
                    std::vector<Pile>& piles, Pile& stock, DealState& dealState);

        void updateDragging(const Vec2& mousePos, bool mouseDown, bool justReleased,
                    std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);

        void updateDealing(std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);
};