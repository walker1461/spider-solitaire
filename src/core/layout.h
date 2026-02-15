#pragma once
#include <vector>

#include "game_config.h"
#include "../model/card.h"
#include "../model/pile.h"

void updateCardPositions(std::vector<Card>& cards, std::vector<Pile>& piles);
bool pointInCard(const Vec2& mouse, const Card& card);
bool pointInPile(const Vec2& mouse, const Pile& pile, const Vec2& cardSize);

void layoutPiles(std::vector<Pile> &piles, const GameConfig& cfg);