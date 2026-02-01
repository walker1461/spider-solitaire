#pragma once
#include <vector>
#include "../model/card.h"
#include "../model/dealState.h"
#include "../math/vectors.h"
#include "../model/pile.h"
#include "../rules/game_rules.h"
#include "../core/game_config.h"

Vec2 screenToWorld(double x, double y, int w, int h);

enum class DragState {
    Idle,
    Dragging
};

class DragController {
public:
    void setRules(GameRules* rules);
    void setConfig(const GameConfig* config);

    void update(const Vec2& mousePos, bool mouseDown, std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock, DealState& dealState);
private:
    GameRules* gameRules = nullptr;
    const GameConfig* gameConfig = nullptr;

    DragState state = DragState::Idle;
    Vec2 dragOffset = {};
    std::vector<int> draggingRun;
    int draggingPile = -1;
    int draggingStartIndex = -1;
    bool wasMouseDown = false;

    void updateIdle(const Vec2& mousePos, bool justPressed, std::vector<Card>& cards, std::vector<Pile>& piles, const Pile& stock, DealState& dealState);
    void updateDragging(const Vec2& mousePos, bool mouseDown, bool justReleased, std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);
    void updateDealing(std::vector<Card>& cards, std::vector<Pile>& piles, Pile& stock);
};