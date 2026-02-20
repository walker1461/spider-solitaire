#include "render.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cardVertices.h"
#include "core/path.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <fstream>
#include <sstream>

static std::unordered_map<std::string, GLuint> textureCache;

std::string importShader(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int createCardVAO() {
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cardVertices), cardVertices, GL_STATIC_DRAW);

    // tell OpenGL the vertex layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
};

unsigned int createShaderProgram() {
    const std::string vertexShaderSource = importShader("assets/shaders/vertexShader.glsl");
    const std::string fragmentShaderSource = importShader("assets/shaders/fragShader.glsl");

    const char* vShaderCode = vertexShaderSource.c_str();
    const char* fShaderCode = fragmentShaderSource.c_str();

    auto compileShader = [](const unsigned int type, const char* src) {
        unsigned int const shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    };

    unsigned int const vertexShader = compileShader(GL_VERTEX_SHADER, vShaderCode);
    unsigned int const fragmentShader = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

    unsigned int const shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
};

void Renderer::init() {
    vao = createCardVAO();
    shader = createShaderProgram();

    cardBackTexture = loadTexture(path::assetPath("cards/Back Red 2.png").string().c_str());
    spaceTexture = loadTexture(path::assetPath("cards/Card Space.png").string().c_str());
    spiderTexture = loadTexture(path::assetPath("cards/spider.png").string().c_str());

    offsetLoc     = glGetUniformLocation(shader, "offset");
    sizeLoc       = glGetUniformLocation(shader, "uSize");
    scaleLoc      = glGetUniformLocation(shader, "uScale");
    radiusLoc     = glGetUniformLocation(shader, "radius");
    aspectLoc     = glGetUniformLocation(shader, "uAspect");
    textureLoc    = glGetUniformLocation(shader, "cardTexture");
    dimmerLoc     = glGetUniformLocation(shader, "uDimAmount");
    alphaLoc      = glGetUniformLocation(shader, "uAlpha");
    tintColorLoc  = glGetUniformLocation(shader, "uTintColor");
    tintAmountLoc = glGetUniformLocation(shader, "uTintAmount");
    rotationLoc   = glGetUniformLocation(shader, "uRotation");
};

void Renderer::render(Game& game) const {
    if (game.state != GameState::MENU) {
        drawSpider();
    }
    drawPiles(game.piles, game.gameConfig.cardSize);
    drawCards(game.cards, game.piles);
}

void Renderer::beginFrame(const int w, const int h) const {
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(vao);
    glUniform2f(aspectLoc, static_cast<float>(w) / static_cast<float>(h), 1.0f);
    glUniform1f(dimmerLoc, 1.0f);
    glUniform3f(tintColorLoc, 1.0f, 0.0f, 0.0f);
    glUniform1f(tintAmountLoc, 0.0f);
};

void Renderer::drawSpider() const {
    glBindTexture(GL_TEXTURE_2D, spiderTexture);
    glUniform1f(scaleLoc, 2.0f);
    glUniform1f(radiusLoc, 0.02f);
    glUniform2f(offsetLoc, 0.0f, 0.0f);
    glUniform2f(sizeLoc, 0.7f, 1.0f);
    glUniform1f(dimmerLoc, 1.0f);
    glUniform1f(alphaLoc, 1.0f);
    glUniform1f(rotationLoc, 0.0f);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawPiles(const std::vector<Pile> &piles, const Vec2 pileSize) const {
    glBindTexture(GL_TEXTURE_2D, spaceTexture);
    glUniform1f(scaleLoc, 1.0f);
    glUniform1f(radiusLoc, 0.02f);
    glUniform1f(rotationLoc, 0.0f);

     for (const auto& pile : piles) {

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUniform1f(radiusLoc, 0.02f);
        glUniform1f(alphaLoc, 1.0f);
        glUniform2f(offsetLoc, pile.basePosition.x, pile.basePosition.y);
        glUniform2f(sizeLoc, pileSize.x, pileSize.y);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    };
};

void Renderer::drawCards(std::vector<Card>& cards, std::vector<Pile>& piles) const{
    std::vector<int> renderOrder; // find z-index order to render cards front to back
    for (int i = 0; i < cards.size(); i++) {
        renderOrder.push_back(i);
    };

    std::sort(renderOrder.begin(), renderOrder.end(), [&](const int a, const int b) {
        // dragging cards always on top
        if (cards[a].isDragging != cards[b].isDragging) return !cards[a].isDragging;

        // lifted cards on top of non-lifted cards
        if (cards[a].isLifted != cards[b].isLifted) return !cards[a].isLifted;

        // sort by pile then position in pile
        if (cards[a].pileIndex != cards[b].pileIndex) return cards[a].pileIndex < cards[b].pileIndex;
        return cards[a].indexInPile < cards[b].indexInPile;
    });

    // get the time passed for animating cards
    static auto lastTime = static_cast<float>(glfwGetTime());
    const auto currentTime = static_cast<float>(glfwGetTime());
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    if (deltaTime > 0.05f) deltaTime = 0.05f;

    // track which lift groups are done moving
    std::unordered_map<int, bool> liftGroupDone;

    std::unordered_set<int> glowDrawnForPile;

    for (const int i : renderOrder) {
        Card& card = cards[i];
        if (!card.isActive) continue;

        if (card.pileIndex >= 0 &&
            card.pileIndex < piles.size())
        {
            const Pile& pile = piles[card.pileIndex];

            if (pile.type == PileType::Tableau)
            {
                int cardCount = static_cast<int>(pile.cardIndices.size());

                if (cardCount > 18)
                {
                    int overflowStart = 18;

                    if (card.indexInPile == overflowStart && glowDrawnForPile.find(card.pileIndex) == glowDrawnForPile.end()) {
                        glowDrawnForPile.insert(card.pileIndex);

                        const int lastIndex = cardCount - 1;

                        // Reconstruct spacing like updateCardPositions
                        constexpr float baseSpacing = 0.07f;
                        constexpr float faceDownBaseSpacing = 0.04f;

                        float spacing = baseSpacing;
                        float faceDownSpacing = faceDownBaseSpacing;

                        constexpr float maxHeight = 1.25f;

                        if (static_cast<float>(cardCount) * spacing > maxHeight) {
                            spacing = maxHeight / static_cast<float>(cardCount);
                            faceDownSpacing = spacing - 0.03f;
                        }

                        // Find last face-down card
                        int lastFaceDownIndex = -1;
                        for (const int idx : pile.cardIndices) {
                            if (!cards[idx].faceUp)
                                lastFaceDownIndex = cards[idx].indexInPile;
                            else
                                break;
                        }

                        const float faceDownOffset =
                            (static_cast<float>(lastFaceDownIndex) + 1.0f) *
                            (spacing - faceDownSpacing);

                        // Compute logical Y positions
                        const float baseY = pile.basePosition.y;

                        auto computeY = [&](const int indexInPile, const bool faceUp) {
                            if (!faceUp) {
                                return baseY - (static_cast<float>(indexInPile) * faceDownSpacing);
                            } else {
                                return baseY - (static_cast<float>(indexInPile) * spacing)
                                       + faceDownOffset;
                            }
                        };

                        const Card& firstOverflowCard =
                            cards[pile.cardIndices[overflowStart]];

                        const Card& lastCard =
                            cards[pile.cardIndices[lastIndex]];

                        const float topY =
                            computeY(firstOverflowCard.indexInPile,
                                     firstOverflowCard.faceUp);

                        const float bottomY =
                            computeY(lastCard.indexInPile,
                                     lastCard.faceUp);

                        const float cardHeight = card.size.y;

                        const float height =
                            (topY - bottomY) + cardHeight;

                        const float centerY =
                            topY - (height * 0.5f)
                            + (cardHeight * 0.5f);

                        // ---- Draw glow ----
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

                        const float pulse =
                            //0.6f + 0.4f * static_cast<float>(sin(glfwGetTime() * 2.5f));
                              0.8f + 0.4f * static_cast<float>(sin(glfwGetTime() * 2.5f));

                        glUniform3f(tintColorLoc, 1.0f, 0.3f, 0.1f);
                        glUniform1f(tintAmountLoc, 0.8f);
                        glUniform1f(alphaLoc, 0.45f * pulse);
                        glUniform1f(radiusLoc, 0.03f);

                        glUniform2f(offsetLoc,
                            pile.basePosition.x,
                            centerY);

                        glUniform2f(sizeLoc,
                            card.size.x * 1.2f,
                            height * 1.08f);

                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                }
            }
        }

        // RESET SHADER STATE
        glUniform3f(tintColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform1f(tintAmountLoc, 0.0f);
        glUniform1f(radiusLoc, 0.02f);
        glUniform1f(alphaLoc, 1.0f);

        if (!card.isDragging && !card.isFlying) {
            const Vec2 delta = card.targetPosition - card.visualPosition;
            card.visualPosition = card.visualPosition + (delta * 12.0f * deltaTime);

            // Check if this card is still moving
            const float dist = abs(card.visualPosition.x - card.targetPosition.x) +
                              abs(card.visualPosition.y - card.targetPosition.y);
            if (card.isLifted && card.liftGroupId > 0) {
                if (liftGroupDone.find(card.liftGroupId) == liftGroupDone.end()) {
                    liftGroupDone[card.liftGroupId] = true;
                }
                if (constexpr float EPSILON = 0.01f; dist > EPSILON) {
                    liftGroupDone[card.liftGroupId] = false;
                }
            }
        }
        renderCard(card, shader, cardBackTexture);
    }

    // clear isLifted for entire groups that have finished moving
    for (auto& card : cards) {
        if (card.isLifted && card.liftGroupId > 0) {
            if (liftGroupDone[card.liftGroupId]) {
                card.isLifted = false;
            }
        }
    }
}

void Renderer::endFrame() {
    glBindVertexArray(0);
}

GLuint getCardTexture(const Card& card) {
    const std::string path = getCardTexturePath(card.suit, card.rank).string();
    if (textureCache.find(path) == textureCache.end()) {
        textureCache[path] = loadTexture(path.c_str());
    }

    return textureCache[path];
}

GLuint loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    if (unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0)) {
        const GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        stbi_image_free(data);
    }
    return textureID;
};

void renderCard(const Card &card, const unsigned int shaderProgram, const unsigned int cardBack) {
    if (card.isDark) {
        glUniform1f(glGetUniformLocation(shaderProgram, "uDimAmount"), 0.75f);
    } else {
        glUniform1f(glGetUniformLocation(shaderProgram, "uDimAmount"), 1.0f);
    }

    glUniform1f(glGetUniformLocation(shaderProgram, "uAlpha"), card.alpha);

    glUniform1f(glGetUniformLocation(shaderProgram, "uRotation"), card.rotation);
    glUniform2f(glGetUniformLocation(shaderProgram, "offset"),
                card.visualPosition.x, card.visualPosition.y);

    glUniform2f(glGetUniformLocation(shaderProgram, "uSize"),
                card.size.x, card.size.y);

    glUniform1f(glGetUniformLocation(shaderProgram, "uScale"), card.scale);
    glUniform1f(glGetUniformLocation(shaderProgram, "radius"), card.cornerRadius);

    glActiveTexture(GL_TEXTURE0);

    if (card.faceUp) {
        glBindTexture(GL_TEXTURE_2D, getCardTexture(card));
    } else {
        glBindTexture(GL_TEXTURE_2D, cardBack);
    }

    glUniform1i(glGetUniformLocation(shaderProgram, "cardTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
};