#pragma once
#include <vector>
#include "../model/card.h"
#include "../model/pile.h"

unsigned int createCardVAO();
unsigned int createShaderProgram();
GLuint loadTexture(const char* path);
void renderCard(const Card &card, unsigned int shaderProgram, unsigned int cardBack);

class Renderer {
public:
    void init();
    void beginFrame(int w, int h) const;
    void drawPiles(const std::vector<Pile>& piles) const;
    void drawCards(std::vector<Card>& cards) const;
    static void endFrame();

private:
    unsigned int vao = 0;
    unsigned int shader = 0;
    GLuint cardBackTexture = 0;
    GLuint spaceTexture = 0;

    int offsetLoc;
    int sizeLoc;
    int scaleLoc;
    int radiusLoc;
    int aspectLoc;
    int textureLoc;
};