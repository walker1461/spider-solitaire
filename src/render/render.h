#pragma once
#include <vector>

#include "glad.h"
#include "../model/card.h"
#include "../model/pile.h"
#include "core/game.h"

unsigned int createCardVAO();
unsigned int createShaderProgram();
GLuint getCardTexture(const Card& card);
GLuint loadTexture(const char* path);
void renderCard(const Card &card, unsigned int shaderProgram, unsigned int cardBack);

class Renderer {
public:
    void init();
    void render(Game& game) const;
    void beginFrame(int w, int h) const;
    void drawSpider() const;
    void drawPiles(const std::vector<Pile>& piles, Vec2 pileSize) const;
    void drawCards(std::vector<Card> &cards, std::vector<Pile>& piles) const;
    static void endFrame();

private:
    unsigned int vao = 0;
    unsigned int shader = 0;
    GLuint cardBackTexture = 0;
    GLuint spaceTexture = 0;
    GLuint spiderTexture = 0;

    int offsetLoc = 0;
    int sizeLoc = 0;
    int scaleLoc = 0;
    int radiusLoc = 0;
    int aspectLoc = 0;
    int textureLoc = 0;
    int dimmerLoc = 0;
    int alphaLoc = 0;
    int tintColorLoc = 0;
    int tintAmountLoc = 0;
    int rotationLoc = 0;
};