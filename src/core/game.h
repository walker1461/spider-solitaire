#pragma once
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include "../input/card_drag.h"

enum GameState {
    NEWGAME,
    MENU,
    PAUSED,
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

    Pile& initializeGame(std::vector<Card>& cards, std::vector<Pile>& cardPiles) const;
    void startNewGame(Difficulty difficulty);
    void update(float deltaTime, const Vec2& mousePos, bool mouseDown, GLFWwindow* window);

    GameState state = GameState::MENU;
    Difficulty difficulty = Difficulty::Normal;

    std::vector<Card> cards;
    std::vector<Pile> piles;
    std::vector<int> pileWarnings{};
    DealState deal;

    std::unique_ptr<GameRules> rules;
    DragController drag;
    GameConfig gameConfig;

    bool hasWon = false;
    bool isPaused = false;
    int score = 0;

    void startDealing();
    void updateDealing(float deltaTime, Pile& stock);
    void updateRunClearAnimation(float deltaTime);
    void cleanupInactiveCards();
    void refillStock();
};

void shuffleDeck(std::vector<Card> &cards);