#include "game.h"
#include "layout.h"
#include "../model/card.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <memory>

#include "../render/render.h"
#include "../rules/spider_rules.h"

constexpr float CARD_HEIGHT = 0.3f;
constexpr float CARD_WIDTH = CARD_HEIGHT * 0.73f;

Pile& Game::initializeGame(std::vector<Card> &deck, std::vector<Pile> &cardPiles) const {
    // ---------- INITIAL DEAL -----------
    shuffleDeck(deck);
    int cardCursor = 0;

    for (int i = 0; i < this->gameConfig.tableauCount; i++) {
        const int count = (i < 4) ? 6 : 5;
        for (int j = 0; j < count; j++) {
            cardPiles[i].cardIndices.push_back(cardCursor);
            deck[cardCursor].size = {CARD_WIDTH, CARD_HEIGHT};
            deck[cardCursor].pileIndex = i;
            deck[cardCursor].indexInPile = j;
            deck[cardCursor].faceUp = (j == count - 1);
            deck[cardCursor].visualPosition = deck[cardCursor].targetPosition;
            cardCursor++;
        }
    }

    // ----------- CREATE STOCK PILE -----------
    cardPiles.emplace_back();
    Pile& stock = cardPiles.back();
    stock.type = PileType::Stock;

    while (cardCursor < static_cast<int>(deck.size())) {
        stock.cardIndices.push_back(cardCursor);

        deck[cardCursor].size = gameConfig.cardSize;
        deck[cardCursor].faceUp = false;
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

void Game::startNewGame(const Difficulty difficulty) {
    const int suitCount = (difficulty == Easy) ? 1 : (difficulty == Normal) ? 2 : 4;
    cards = generateDeck(suitCount);

    rules = std::make_unique<SpiderRules>();
    gameConfig = rules->config();

    piles.clear();
    piles.resize(gameConfig.tableauCount);

    initializeGame(cards, piles);
    layoutPiles(piles, gameConfig);

    drag.setRules(rules.get());
    drag.setConfig(&gameConfig);

    state = GameState::GAME;
}

void Game::update(const float deltaTime, const Vec2& mousePos, const bool mouseDown) {
    switch (state) {
        case GameState::GAME:
            updateDealing(deltaTime, piles[gameConfig.stockPileIndex]);
            drag.update(mousePos, mouseDown, cards, piles, piles[gameConfig.stockPileIndex], deal);
            updateCardPositions(cards, piles);
            if (rules) rules->darkenCards(cards, piles);
            break;
        default:
            break;
    }
}

void Game::render(const Renderer& renderer) {
    renderer.drawSpider();
    renderer.drawPiles(piles, gameConfig.cardSize);
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
    if (stock.cardIndices.empty() || deal.nextPile >= this->gameConfig.tableauCount) {
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