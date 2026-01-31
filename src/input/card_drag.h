#pragma once
#include <vector>
#include "../model/card.h"
#include "../model/dealState.h"
#include "../math/vectors.h"
#include "../model/pile.h"

Vec2 screenToWorld(double x, double y, int w, int h);
void moveRun(std::vector<Card>& cards, std::vector<Pile>& piles, int fromPile, int toPile, int startIndex);

enum class DragState {
    Idle,
    Dragging
};

class DragController {
    public:
        DragState state = DragState::Idle;

        std::vector<int> draggingRun;
        int draggingPile = -1;
        int draggingStartIndex = -1;
        Vec2 dragOffset;
        bool wasMouseDown = false;

        void update(const Vec2& mousePos, bool mouseDown, std::vector<Card>& cards,
                    std::vector<Pile>& piles, Pile& stock, DealState& dealState);
    private:
        void updateIdle(const Vec2& mousePos, bool justPressed, std::vector<Card>& cards,
                    std::vector<Pile>& piles, const Pile& stock, DealState& dealState);

        void updateDragging(const Vec2& mousePos, bool mouseDown, bool justReleased,
                    std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);

        void updateDealing(std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);
};