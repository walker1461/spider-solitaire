#include "render.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cardVertices.h"
#include <algorithm>
#include <unordered_map>

#include "GLFW/glfw3.h"

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
    const auto vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        uniform vec2 offset;
        uniform vec2 uSize;
        uniform float uScale;
        uniform vec2 uAspect;

        out vec2 TexCoords;
        out vec2 localPos;

        void main() {
            localPos = aPos.xy * uSize * uScale;

            vec2 aspectPos = localPos;
            float aspect = uAspect.x / uAspect.y;
            aspectPos.x /= aspect;

            gl_Position = vec4(aspectPos + offset, aPos.z, 1.0);
            TexCoords = aTexCoord;
        }
    )";

    const auto fragmentShaderSource = R"(
        #version 330 core
        in vec2 localPos;
        in vec2 TexCoords;
        out vec4 FragColor;

        uniform sampler2D cardTexture;
        uniform vec3 uColor;
        uniform float radius;
        uniform vec2 uSize;
        uniform float uScale;
        uniform float uDimAmount;

        void main() {
            vec2 halfSize = 0.5 * uSize * uScale;
            float r = radius * uScale;

            vec2 p = abs(localPos);
            vec2 q = p - halfSize + vec2(r);
            float dist = length(max(q, 0.0));

            vec4 texColor = texture(cardTexture, TexCoords);

            if (dist > r) {
                discard;
            }

            FragColor = vec4(texColor.rgb * uDimAmount, texColor.a);
        }
    )";

    auto compileShader = [](const unsigned int type, const char* src) {
        unsigned int const shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    };

    unsigned int const vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int const fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

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

    cardBackTexture = loadTexture("textures/Back Red 2.png");
    spaceTexture = loadTexture("textures/Card Space.png");
    spiderTexture = loadTexture("textures/spider.png");

    offsetLoc  = glGetUniformLocation(shader, "offset");
    sizeLoc    = glGetUniformLocation(shader, "uSize");
    scaleLoc   = glGetUniformLocation(shader, "uScale");
    radiusLoc  = glGetUniformLocation(shader, "radius");
    aspectLoc  = glGetUniformLocation(shader, "uAspect");
    textureLoc = glGetUniformLocation(shader, "cardTexture");
    dimmerLoc  = glGetUniformLocation(shader, "uDimAmount");
};

void Renderer::beginFrame(const int w, const int h) const {
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(vao);
    glUniform2f(aspectLoc, static_cast<float>(w) / static_cast<float>(h), 1.0f);
    glUniform1f(dimmerLoc, 1.0f);
};

void Renderer::drawSpider() const {
    glBindTexture(GL_TEXTURE_2D, spiderTexture);
    glUniform1f(scaleLoc, 2.0f);
    glUniform1f(radiusLoc, 0.02f);
    glUniform2f(offsetLoc, 0.0f, 0.0f);
    glUniform2f(sizeLoc, 0.7f, 1.0f);
    glUniform1f(dimmerLoc, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawPiles(const std::vector<Pile> &piles, const Vec2 pileSize) const {
    glBindTexture(GL_TEXTURE_2D, spaceTexture);
    glUniform1f(scaleLoc, 1.0f);
    glUniform1f(radiusLoc, 0.02f);
    glUniform1f(dimmerLoc, 1.0f);

    for (const auto& pile : piles) {
        glUniform2f(offsetLoc, pile.basePosition.x, pile.basePosition.y);
        glUniform2f(sizeLoc, pileSize.x, pileSize.y);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };
};

void Renderer::drawCards(std::vector<Card>& cards) const{
    std::vector<int> renderOrder; // find z-index order to render cards front to back
    for (int i = 0; i < cards.size(); i++) {
        renderOrder.push_back(i);
    };

    //const bool initialDealPhase = (cardsStillMoving > 10);
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
    constexpr float EPSILON = 0.01f;

    for (const int i : renderOrder) {
        Card& card = cards[i];
        if (!card.isDragging) {
            const Vec2 delta = card.targetPosition - card.visualPosition;
            card.visualPosition = card.visualPosition + (delta * 12.0f * deltaTime);

            // Check if this card is still moving
            const float dist = abs(card.visualPosition.x - card.targetPosition.x) +
                              abs(card.visualPosition.y - card.targetPosition.y);
            if (card.isLifted && card.liftGroupId > 0) {
                if (liftGroupDone.find(card.liftGroupId) == liftGroupDone.end()) {
                    liftGroupDone[card.liftGroupId] = true;
                }
                if (dist > EPSILON) {
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

    glUniform2f(glGetUniformLocation(shaderProgram, "offset"),
                card.visualPosition.x, card.visualPosition.y);

    glUniform2f(glGetUniformLocation(shaderProgram, "uSize"),
                card.size.x, card.size.y);

    glUniform1f(glGetUniformLocation(shaderProgram, "uScale"), card.scale);
    glUniform1f(glGetUniformLocation(shaderProgram, "radius"), card.cornerRadius);

    glActiveTexture(GL_TEXTURE0);
    if (card.faceUp) {
        glBindTexture(GL_TEXTURE_2D, card.textureID);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardBack);
    }
    glUniform1i(glGetUniformLocation(shaderProgram, "cardTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
};