#include "game.h"
#include "layout.h"
#include "../model/card.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <memory>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "persistence.h"
#include "../rules/spider_rules.h"
#include "ui/main_menu.h"
#include "ui/pause_menu.h"

constexpr float CARD_HEIGHT = 0.3f;
constexpr float CARD_WIDTH = CARD_HEIGHT * 0.73f;

Pile& Game::initializeGame(std::vector<Card>& cards, std::vector<Pile>& cardPiles) const {
    // ---------- INITIAL DEAL -----------
    shuffleDeck(cards);
    int cardCursor = 0;

    for (int i = 0; i < this->gameConfig.tableauCount; i++) {
        const int count = (i < 4) ? 6 : 5;
        for (int j = 0; j < count; j++) {
            cardPiles[i].cardIndices.push_back(cardCursor);
            cards[cardCursor].size = {CARD_WIDTH, CARD_HEIGHT};
            cards[cardCursor].pileIndex = i;
            cards[cardCursor].indexInPile = j;
            cards[cardCursor].faceUp = (j == count - 1);
            cards[cardCursor].visualPosition = cards[cardCursor].targetPosition;
            cardCursor++;
        }
    }

    // ----------- CREATE STOCK PILE -----------
    cardPiles.emplace_back();
    Pile& stock = cardPiles.back();
    stock.type = PileType::Stock;

    while (cardCursor < static_cast<int>(this->cards.size())) {
        stock.cardIndices.push_back(cardCursor);

        cards[cardCursor].size = gameConfig.cardSize;
        cards[cardCursor].faceUp = false;
        cardCursor++;
    }

    // --------- CREATE COMPLETED PILE SPACES ---------
    for (int i = 0; i < this->gameConfig.completedPileCount; i++) {
        cardPiles.emplace_back();
        Pile& completedPile = cardPiles.back();
        completedPile.type = PileType::Completed;
    }

    // tableau piles        stock     completed piles     //
    // 0 1 2 3 4 5 6 7 8 9 - 10 - 11 12 13 14 15 16 17 18 //
    return stock;
}

void Game::initHighScores() {
    highScores = loadScores();
}

void Game::updateHighScores() {
    highScores.push_back(score);
    std::sort(highScores.rbegin(), highScores.rend());

    if (highScores.size() > 5) {
        highScores.resize(5);
    }
}

void Game::startNewGame(const Difficulty difficulty) {
    this->difficulty = difficulty;
    //const int suitCount = (difficulty == Easy) ? 1 : (difficulty == Normal) ? 2 : 4;
    //cards = generateDeck(suitCount);
    this->cards = generateDeck(8);

    rules = std::make_unique<SpiderRules>();

    gameConfig = rules->config();

    if (difficulty == Difficulty::Easy) {
        gameConfig.maxPileSize = 20;
    } else if (difficulty == Difficulty::Normal) {
        gameConfig.maxPileSize = 18;
    } else if (difficulty == Difficulty::Hard) {
        gameConfig.maxPileSize = 16;
    };

    piles.clear();
    piles.resize(gameConfig.tableauCount);
    score = 0;

    initializeGame(this->cards, piles);
    layoutPiles(piles, gameConfig);

    drag.setRules(rules.get());
    drag.setConfig(&gameConfig);

    state = GameState::GAME;
}

void Game::update(const float deltaTime, const Vec2& mousePos, const bool mouseDown, GLFWwindow* window) {
    // check for escape button press
    static float lastEscPress{};
    const float escDeltaTime = static_cast<float>(glfwGetTime()) - lastEscPress;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        lastEscPress = static_cast<float>(glfwGetTime());
        if (escDeltaTime > 0.1f && state != GameState::MENU) isPaused = !isPaused;
        if (isPaused) {
            state = GameState::PAUSED;
        } else {
            state = GameState::GAME;
        }
    }

    DropResult result;

    switch (state) {
        case GameState::GAME: {
            updateDealing(deltaTime, piles[gameConfig.stockPileIndex]);
            auto dropEvent = drag.update(mousePos, mouseDown, this->cards, piles,
                                        piles[gameConfig.stockPileIndex], deal, deltaTime);
            if (dropEvent) {
                result = rules->onDrop(cards, piles, dropEvent->fromPile, dropEvent->toPile, dropEvent->startIndex);
                if (result.runCleared) {
                    score += 100;
                }
            }

            updateCardPositions(this->cards, piles);
            updateRunClearAnimation(deltaTime);

            rules->checkPileSizes(piles);

            if (rules) rules->darkenCards(this->cards, piles);
            break;
        }
        case GameState::MENU: {
            showMenu(state, difficulty, highScores);
            break;
        }
        case GameState::PAUSED: {
            drawPauseBackground();
            showPauseMenu(state, isPaused);
            break;
        }
        case GameState::NEWGAME: {
            startNewGame(difficulty);
            state = GameState::GAME;
            break;
        }
        case GameState::LOSER: {
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::Begin("Game Over!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

            if (ImGui::Button("Play Again", ImVec2(140, 25))) {
                state = GameState::NEWGAME;
                gameOver = false;
            }
            if (ImGui::Button("Main Menu", ImVec2(140, 25))) {
                state = GameState::MENU;
                gameOver = false;
            }
            ImGui::End();
            break;
        }
        case GameState::QUIT: {
            break;
        }
    }
}

void shuffleDeck(std::vector<Card>& cards) {
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
}

// void Game::startDealing() {
//     deal.active = true;
//     deal.timer = 0.0f;
//     deal.nextPile = 0;
// }

void Game::updateDealing(const float deltaTime, Pile& stock) {
    if (!deal.active) return;

    deal.timer += deltaTime;
    if (deal.timer < deal.delay) return;
    deal.timer = 0.0f;

    if (stock.cardIndices.empty() || deal.nextPile >= this->gameConfig.tableauCount) {
        deal.active = false;
        if (rules->checkForGameOver(piles, gameConfig.maxPileSize)) state = GameState::LOSER;
        updateHighScores();
        saveScores(highScores);
        return;
    }

    const int cardIndex = stock.cardIndices.back();
    stock.cardIndices.pop_back();

    Card& card = cards[cardIndex];
    card.faceUp = true;
    card.pileIndex = deal.nextPile;
    card.indexInPile = static_cast<int>(piles[deal.nextPile].cardIndices.size());

    card.visualPosition = stock.basePosition;
    card.targetPosition = piles[deal.nextPile].basePosition;

    static int nextLiftGroupId = 100000;
    card.isDragging = false;
    card.isLifted = true;
    card.liftGroupId = nextLiftGroupId++;

    piles[deal.nextPile].cardIndices.push_back(cardIndex);

    deal.nextPile++;

    //check if stock pile needs refilling
    if (piles[gameConfig.stockPileIndex].cardIndices.size() < gameConfig.tableauCount) {
        refillStock();
    }
}

void Game::updateRunClearAnimation(const float deltaTime) {
    for (Card& card : cards) {
        if (!card.isFlying) continue;

        card.flyTimer += deltaTime;

        float t = card.flyTimer / card.flyDuration;
        if (t > 1.0f) t = 1.0f;

        // cubic ease
        const float ease = 1.0f - pow(1.0f - t, 3.0f);

        card.visualPosition =
            card.flyStart +
            (card.flyTarget - card.flyStart) * ease;

        card.rotation += ((6.28318f * deltaTime) - t);

        // fade only near end
        float fadeStart = 0.7f;

        if (t < fadeStart) {
            card.alpha = 1.0f;
        } else {
            float fadeT = (t - fadeStart) / (1.0f - fadeStart);
            card.alpha = 1.0f - fadeT;
        }

        if (t >= 1.0f) {
            card.isFlying = false;
            card.alpha = 0.0f;
            card.isActive = false;
            card.pileIndex = -1;
            card.indexInPile = -1;
        }
    }
    cleanupInactiveCards();
}

void Game::cleanupInactiveCards() {
    for (auto& pile : piles) {
        pile.cardIndices.erase(
            std::remove_if(
                pile.cardIndices.begin(),
                pile.cardIndices.end(),
                [&](const int index) {
                    return !cards[index].isActive;
                }),
            pile.cardIndices.end()
        );
    }
}

void Game::refillStock() {
    std::cout << "REFILL CALLED\n";

    std::vector<Card> newCards = generateDeck(8);
    shuffleDeck(newCards);

    Pile& stock = piles[gameConfig.stockPileIndex];

    for (auto& newCard : newCards) {
        int newIndex = static_cast<int>(cards.size());

        cards.push_back(newCard);

        Card& card = cards[newIndex];

        card.faceUp = false;
        card.pileIndex = gameConfig.stockPileIndex;
        card.indexInPile = static_cast<int>(stock.cardIndices.size());
        card.size = gameConfig.cardSize;
        card.visualPosition = stock.basePosition;
        card.targetPosition = stock.basePosition;

        stock.cardIndices.push_back(newIndex);
    }
    std::cout << piles[gameConfig.stockPileIndex].cardIndices.size() << std::endl;
}