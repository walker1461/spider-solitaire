#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "vectors.h"
#include "card.h"
#include "render.h"
#include "card_drag.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>
#include <ostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
};

Vec2 screenToWorld(const double x, const double y, const int w, const int h) {
	const auto ndcX = static_cast<float>(2.0 * x / w - 1.0);
	const auto ndcY = static_cast<float>(1.0 - 2.0 * y / h);
	return {ndcX, ndcY};
};

GLFWwindow* openGlInit(const float main_scale) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(static_cast<int>(800 * main_scale), static_cast<int>(600 * main_scale), "Spider Solitaire", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    return window;
};

void SetupImGuiStyle(const float main_scale, const ImVec4 clear_color)
{
	// Comfy style by Giuseppe from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.1f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 10.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(30.0f, 30.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 5.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 10.0f;
	style.PopupBorderSize = 0.0f;
	style.FramePadding = ImVec2(5.0f, 3.5f);
	style.FrameRounding = 5.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(5.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 5.0f;
	style.ColumnsMinSpacing = 5.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 15.0f;
	style.GrabRounding = 5.0f;
	style.TabRounding = 5.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.360515f);
	style.Colors[ImGuiCol_WindowBg] = clear_color;
	style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.42352942f, 0.38039216f, 0.57254905f, 0.5493562f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38039216f, 0.42352942f, 0.57254905f, 0.54901963f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.25882354f, 0.25882354f, 0.25882354f, 0.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.23529412f, 0.23529412f, 0.23529412f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.4509804f, 1.0f, 0.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13333334f, 0.25882354f, 0.42352942f, 0.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.42352942f, 0.38039216f, 0.57254905f, 0.54901963f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.42352942f, 0.38039216f, 0.57254905f, 0.2918455f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03433478f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);

	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;
}

void shuffleDeck(std::vector<Card>& cards) {
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
}

constexpr float CARD_HEIGHT = 0.3f;
constexpr float CARD_WIDTH = CARD_HEIGHT * 0.73f;
constexpr float TABLEAU_BOTTOM = -0.9f;

Pile& initializeGame(std::vector<Card> &cards, std::vector<Pile> &piles) {
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

enum GameState {
	NEWGAME,
    MENU,
    GAME,
    WINNER,
    QUIT
};

enum Difficulty {
	Easy,
	Normal,
	Hard
};

void showMenu(GameState& gameState, Difficulty& difficulty) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(300, 345));

    ImGui::Begin("Main Menu", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImGui::PushFont(ImGui::GetFont());

	float textWidth = ImGui::CalcTextSize("Spider Solitaire").x;
	float windowWidth = ImGui::GetWindowSize().x;
	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

	ImGui::Text("Spider Solitaire");
	ImGui::PopFont();

	ImGui::BeginChild("DifficultyBox", ImVec2(0, 160), true);
	    ImGui::Text("Choose a difficulty:");
		ImGui::Separator();
		if (ImGui::RadioButton("Easy", difficulty == Easy)) difficulty = Easy;
		if (ImGui::RadioButton("Normal", difficulty == Normal)) difficulty = Normal;
		if (ImGui::RadioButton("Hard", difficulty == Hard)) difficulty = Hard;
	ImGui::EndChild();

	ImGui::Dummy(ImVec2(0, 10));
	if (ImGui::Button("Play", ImVec2(-1, 40))) {
		gameState = NEWGAME;
	}

	ImGui::Dummy(ImVec2(0, 5));
	if (ImGui::Button("Quit", ImVec2(-1, 30))) {
		gameState = QUIT;
	}

    ImGui::End();
	ImGui::PopStyleVar(3);
}

void layoutPiles(std::vector<Pile> &piles) {
	constexpr float marginX = 0.08f;
	constexpr float topY = 0.75f;

	constexpr float usableWidth = 2.0f - marginX * 2.0f;
	constexpr float spacing = usableWidth / 9.0f;

	// tableaus
	for (int i = 0; i < 10; i++) {
		constexpr float tableauY = 0.35f;
		piles[i].basePosition = {
			-1.0f + marginX + spacing * static_cast<float>(i),
			tableauY
		};
	}

	// stock
	piles[10].basePosition = { 0.9f, topY };

	// completed piles
	for (int i = 0; i < 8; i++) {
		piles[11 + i].basePosition = {
			-1.0f + marginX + spacing * static_cast<float>(i),
			topY
		};
	}
}


constexpr float VIRTUAL_WIDTH = 2.0f;
constexpr float VIRTUAL_HEIGHT = 2.0f;

int main() {

    // ---------- GET OPENGL WINDOW ----------
    if (!glfwInit()) return -1;
    const float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = openGlInit(main_scale);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) return -1;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --------- SETUP DEAR IMGUI CONTEXT
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.FontGlobalScale = 1.3f;


    // ---------- SETUP SCALING ----------
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
    SetupImGuiStyle(main_scale, clear_color);

    // ---------- SETUP PLATFORM/RENDERER BACKENDS
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    const auto vao = createCardVAO();
    const auto shader = createShaderProgram();

    // ---------- GET SHADER UNIFORMS ----------
    const int offsetLoc      = glGetUniformLocation(shader, "offset");
    const int scaleLoc       = glGetUniformLocation(shader, "uScale");
    const int radiusLoc      = glGetUniformLocation(shader, "radius");
    const int sizeLoc        = glGetUniformLocation(shader, "uSize");
    const int cardTextureLoc = glGetUniformLocation(shader, "cardTexture");
	const int aspectLoc      = glGetUniformLocation(shader, "uAspect");

    const GLuint spaceTexture = loadTexture("textures/Card Space.png");
    const GLuint cardBack = loadTexture("textures/Back Red 2.png");

    // ---------- INIT SPACES & CARDS -----------
    std::vector<Pile> piles(10);
    std::vector<Card> cards;
	Pile* stock = nullptr;

    // ----------- GAME LOGIC SETUP ---------
    DragController dragController;
    GameState currentState = MENU;
	Difficulty difficulty = Easy;
	DealState dealState;

    // ----------- GAME LOOP -----------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    	int window_width, window_height;
    	glfwGetFramebufferSize(window, &window_width, &window_height);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            currentState = QUIT;
        }

    	static auto lastTime = static_cast<float>(glfwGetTime());
    	const auto currentTime = static_cast<float>(glfwGetTime());
    	float deltaTime = currentTime - lastTime;
    	lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ---------- HANDLE MOUSE INPUT -----------
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        const bool leftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        Vec2 mousePosition = screenToWorld(mouseX, mouseY, window_width, window_height);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        switch (currentState) {
            case MENU:
                showMenu(currentState, difficulty);
                break;

        	case NEWGAME: {
        		int numSuits = 0;

        		if (difficulty == Difficulty::Easy) numSuits = 1;
        		else if (difficulty == Difficulty::Normal) numSuits = 2;
        		else if (difficulty == Difficulty::Hard) numSuits = 4;

        		cards = generateDeck(numSuits);
        		stock = &initializeGame(cards, piles);
        		currentState = GAME;

        		break;
        	}

            case GAME: {
        		if (!stock) break;
        		layoutPiles(piles);

        		std::vector<int> renderOrder; // find z-index order to render cards front to back
        		for (int i = 0; i < cards.size(); i++) {
        			renderOrder.push_back(i);
        		}
        		std::sort(renderOrder.begin(), renderOrder.end(), [&](const int a, const int b) {
					if (cards[a].isDragging != cards[b].isDragging) return !cards[a].isDragging;
        			return cards[a].indexInPile < cards[b].indexInPile;
				});

        		updateCardPositions(cards, piles); // sync any changes to cards and piles
        		updateDealing(dealState ,deltaTime, cards, piles, *stock);
        		dragController.update(mousePosition, leftMousePressed, cards, piles, *stock, dealState);

        		glUseProgram(shader);
        		glBindVertexArray(vao);
        		glActiveTexture(GL_TEXTURE0);
        		glBindTexture(GL_TEXTURE_2D, spaceTexture);
        		glUniform1i(cardTextureLoc, 0);
        		glUniform2f(aspectLoc, static_cast<float>(window_width) / static_cast<float>(window_height), 1.0f);

        		for (const auto &pile : piles) { // draw the piles under the cards
        			glUniform2f(offsetLoc, pile.basePosition.x, pile.basePosition.y);
        			glUniform2f(sizeLoc, 0.22f, 0.3f);
        			glUniform1f(scaleLoc, 1.0f);
        			glUniform1f(radiusLoc, 0.02f);

        			glDrawArrays(GL_TRIANGLES, 0, 6);
        		}

        		for (const int i : renderOrder) { // draw the cards
        			Card& card = cards[i];
        			if (!card.isDragging) {
        				const Vec2 delta = card.targetPosition - card.visualPosition;
        				card.visualPosition = card.visualPosition + (delta * 12.0f * deltaTime);

        				if ((delta.x * delta.x + delta.y * delta.y) < 0.002f * 0.002f) {
        					card.visualPosition = card.targetPosition;
        				}

        			}
        			renderCard(card, shader, vao, cardBack);
        		}
        		break;
        	}

        	case WINNER:
               //youWin();
        		std::cout << "You win!" << std::endl;
        		glfwSetWindowShouldClose(window, true);
				break;

            case QUIT:
				glfwSetWindowShouldClose(window, true);
				break;
        }
        // ----------- DRAW TO SCREEN ------------

        ImGui::Render();
        glViewport(0, 0, window_width, window_height);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}