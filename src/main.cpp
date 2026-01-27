#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "math.h"
#include "card.h"
#include "render.h"
#include "card_drag.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
};

GLFWwindow* openGlInit() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello World", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    return window;
};

void shuffleDeck(std::vector<Card>& cards) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = openGlInit();

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) return -1;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    const GLuint spaceTexture = loadTexture("textures/Card Space.png");

    const auto vao = createCardVAO();
    const auto shader = createShaderProgram();

    const int offsetLoc      = glGetUniformLocation(shader, "offset");
    const int scaleLoc       = glGetUniformLocation(shader, "uScale");
    const int radiusLoc      = glGetUniformLocation(shader, "radius");
    const int sizeLoc        = glGetUniformLocation(shader, "uSize");
    const int cardTextureLoc = glGetUniformLocation(shader, "cardTexture");

    const GLuint cardBack = loadTexture("textures/Back Red 2.png");

    // --------- INIT SPACES & CARDS ----------
    Pile stock;
    stock.type = PileType::Stock;
    stock.basePosition = {0.9f, 0.82f};

    std::vector<Pile> piles(11);
    std::vector<Card> cards;
    cards.reserve(104);
    cards = generateDeck(4);
    shuffleDeck(cards);

    // ---------- INITIAL DEAL -----------
    int cardCursor = 0;

    for (int i = 0; i < 10; i++) {
        const int count = (i < 4) ? 6 : 5;
        for (int j = 0; j < count; j++) {
            piles[i].cardIndices.push_back(cardCursor);
            cards[cardCursor].size = {0.18f, 0.3f};
            cards[cardCursor].pileIndex = i;
            cards[cardCursor].indexInPile = j;
            cards[cardCursor].faceUp = (j == count - 1);
            cardCursor++;
        }
    }

    while (cardCursor < 104) {
        stock.cardIndices.push_back(cardCursor);
        cardCursor++;
        cards[cardCursor].size = {0.18f, 0.3f};
        cards[cardCursor].faceUp = false;
        cards[cardCursor].textureID = cardBack;
    }

    piles.push_back(stock);

    for (int i = 0; i < 10; i++) {
        constexpr float spacing = 0.2f;
        constexpr float startX = -0.9f;
        constexpr float pileY = 0.4f;
        piles[i].basePosition = {startX + i * spacing, pileY};
    };

    DragController dragController;
    // -------- RENDER LOOP ---------
    while (!glfwWindowShouldClose(window)) {

        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // CLEAR SCREEN
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // GET MOUSE POSITION AND CONVERT TO OPENGL COORDS
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        const float mouseX_NDC = (2.0f * mouseX) / window_width - 1.0f;
        const float mouseY_NDC = 1.0f - (2.0f * mouseY) / window_height;

        // CHECK FOR DRAGGING
        const bool leftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        Vec2 mousePosition = {mouseX_NDC, mouseY_NDC};
        dragController.update(mousePosition, leftMousePressed, cards, piles);

        // ---------- DRAW TO SCREEN ------------
        glUseProgram(shader);
        glBindVertexArray(vao);

        std::vector<int> renderOrder;
        for (int i = 0; i < cards.size(); i++) {
            renderOrder.push_back(i);
        }
        std::sort(renderOrder.begin(), renderOrder.end(), [&](const int a, const int b) {
            return cards[a].indexInPile < cards[b].indexInPile;
        });

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, spaceTexture);
        glUniform1i(cardTextureLoc, 0);

        updateCardPositions(cards, piles);

        assert (offsetLoc != -1);
        assert(vao != -1);
        assert(shader != -1);

        for (const auto &pile : piles) {
            glUniform2f(offsetLoc, pile.basePosition.x, pile.basePosition.y);
            glUniform2f(sizeLoc, 0.18f, 0.3f);
            glUniform1f(scaleLoc, 1.0f);
            glUniform1f(radiusLoc, 0.02f);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        for (const int i : renderOrder) {
            renderCard(cards[i], shader, vao, cardBack);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}