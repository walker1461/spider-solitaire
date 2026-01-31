#pragma once
#include <vector>
#include "../model/card.h"
#include "../model/pile.h"
#include "../model/dealState.h"
#include "../input/card_drag.h"
#include "../render/render.h"

enum GameState {
    NEWGAME,
    MENU,
    GAME,
    WINNER,
    QUIT
};

enum Difficulty {
    Easy,
    Normal,
    Hard
};

struct Game {
    Game() = default;

    static Pile& initializeGame(std::vector<Card> &cards, std::vector<Pile> &piles);
    void startNewGame(Difficulty difficulty);
    void update(float deltaTime, const Vec2& mousePos, bool mouseDown);
    void render(const Renderer& renderer);

    GameState state = GameState::MENU;

private:
    std::vector<Card> cards;
    std::vector<Pile> piles;
    DealState deal;
    DragController drag;

    int stockPileIndex = -1;

    void startDealing();
    void updateDealing(float deltaTime, Pile& stock);
};

void shuffleDeck(std::vector<Card> &cards);