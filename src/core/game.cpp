#include "game.h"
#include "layout.h"
#include "../model/card.h"
#include <random>
#include <chrono>
#include <algorithm>

constexpr float CARD_HEIGHT = 0.3f;
constexpr float CARD_WIDTH = CARD_HEIGHT * 0.73f;

Pile& Game::initializeGame(std::vector<Card> &cards, std::vector<Pile> &piles) {
    // ---------- INITIAL DEAL -----------
    shuffleDeck(cards);
    int cardCursor = 0;

    for (int i = 0; i < 10; i++) {
        const int count = (i < 4) ? 6 : 5;
        for (int j = 0; j < count; j++) {
            piles[i].cardIndices.push_back(cardCursor);
            cards[cardCursor].size = {CARD_WIDTH, CARD_HEIGHT};
            cards[cardCursor].pileIndex = i;
            cards[cardCursor].indexInPile = j;
            cards[cardCursor].faceUp = (j == count - 1);
            cards[cardCursor].visualPosition = cards[cardCursor].targetPosition;
            cardCursor++;
        }
    }

    // ----------- CREATE STOCK PILE -----------
    piles.emplace_back();
    Pile& stock = piles.back();
    stock.type = PileType::Stock;
    stock.basePosition = {0.9f, 0.8f};

    while (cardCursor < 104) {
        stock.cardIndices.push_back(cardCursor);

        cards[cardCursor].size = {0.22f, 0.3f};
        cards[cardCursor].faceUp = false;
        cardCursor++;
    }

    // --------- CREATE COMPLETED PILE SPACES ---------
    for (int i = 0; i < 8; i++) {
        constexpr float startX = -0.9f;
        constexpr float spacing = 0.2f;
        piles.emplace_back();
        Pile& completedPile = piles.back();
        completedPile.type = PileType::Completed;
        completedPile.basePosition.x = startX + static_cast<float>(i) * spacing;
        completedPile.basePosition.y = 0.8f;
    }

    // tableau piles        stock     completed piles     //
    // 0 1 2 3 4 5 6 7 8 9 - 10 - 11 12 13 14 15 16 17 18 //
    return stock;
}

void Game::startNewGame(Difficulty difficulty) {
    const int suitCount = (difficulty == Easy) ? 1 : (difficulty == Normal) ? 2 : 4;
    cards = generateDeck(suitCount);
    piles.clear();
    piles.resize(10);
    initializeGame(cards, piles);
    layoutPiles(piles);
    stockPileIndex = 10;
    state = GameState::GAME;
}

void Game::update(const float deltaTime, const Vec2& mousePos, const bool mouseDown) {
    switch (state) {
        case GameState::GAME:
            updateDealing(deltaTime, piles[stockPileIndex]);
            drag.update(mousePos, mouseDown, cards, piles, piles[stockPileIndex], deal);
            updateCardPositions(cards, piles);
            break;
        default:
            break;
    }
}

void Game::render(const Renderer& renderer) {
    renderer.drawPiles(piles);
    renderer.drawCards(cards);
}

void shuffleDeck(std::vector<Card> &cards) {
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
}

void Game::startDealing() {
    deal.active = true;
    deal.timer = 0.0f;
    deal.nextPile = 0;
}

void Game::updateDealing(const float deltaTime, Pile& stock) {
    if (!deal.active) return;
    deal.timer += deltaTime;
    if (deal.timer < deal.delay) return;
    deal.timer = 0.0f;
    if (stock.cardIndices.empty() || deal.nextPile >= 10) {
        deal.active = false;
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

    piles[deal.nextPile].cardIndices.push_back(cardIndex);
    deal.nextPile++;
}