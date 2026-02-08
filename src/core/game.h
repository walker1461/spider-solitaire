#pragma once
#include <vector>
#include <memory>
#include "../model/card.h"
#include "../model/pile.h"
#include "../model/dealState.h"
#include "../input/card_drag.h"
#include "../core/game_config.h"
#include "../model/autoState.h"

class Renderer;

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

    Pile& initializeGame(std::vector<Card> &deck, std::vector<Pile> &cardPiles) const;
    void startNewGame(Difficulty difficulty);
    void update(float deltaTime, const Vec2& mousePos, bool mouseDown);
    void render(const Renderer& renderer);

    GameState state = GameState::MENU;

    std::vector<Card> cards;
    std::vector<Pile> piles;
    DealState deal;
    AutoState autoState;
    std::unique_ptr<GameRules> rules;
    DragController drag;

    GameConfig gameConfig;
    bool hasWon = false;

    void startDealing();
    void updateDealing(float deltaTime, Pile& stock);
    void updateAutoComplete(float deltaTime);
};

void shuffleDeck(std::vector<Card> &cards);