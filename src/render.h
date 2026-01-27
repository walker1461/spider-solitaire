#pragma once
#include "card.h"
#include "glad/glad.h"

unsigned int createCardVAO();
unsigned int createShaderProgram();
GLuint loadTexture(const char* path);
void renderCard(const Card& card, unsigned int shaderProgram, unsigned int VAO, unsigned int cardBack);