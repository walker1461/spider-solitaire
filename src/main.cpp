#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/game.h"
#include "input/card_drag.h"
#include "input/input_state.h"
#include "render/render.h"

#include <random>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
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

void showMenu(GameState& gameState, Difficulty& difficulty) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(300, 345));

    ImGui::Begin("Main Menu", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImGui::PushFont(ImGui::GetFont());

	const float textWidth = ImGui::CalcTextSize("Spider Solitaire").x;
	const float windowWidth = ImGui::GetWindowSize().x;
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

    // ---------- SETUP PLATFORM/RENDERER BACKENDS
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // ---------- INIT RENDERER & GAME -----------
    Renderer renderer;
    renderer.init();

    Game game;
    Difficulty difficulty = Normal;

    static auto lastTime = static_cast<float>(glfwGetTime());
    constexpr float menuCoolDown = 0.1f;

    // ----------- GAME LOOP -----------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (game.state == QUIT)
            break;

    	int window_width, window_height;
    	glfwGetFramebufferSize(window, &window_width, &window_height);
    	int screen_width, screen_height;
    	glfwGetWindowSize(window, &screen_width, &screen_height);

    	const auto currentTime = static_cast<float>(glfwGetTime());
    	const float deltaTime = currentTime - lastTime;
    	lastTime = currentTime;
    	static float lastEscPress{};
    	const float escDeltaTime = currentTime - lastEscPress;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ---------- HANDLE MOUSE INPUT -----------
        static bool lastMouseDown = false;
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        const bool mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        InputState input{
            .mouseWorld = screenToWorld(mouseX, mouseY, screen_width, screen_height),
            .mouseDown = mouseDown,
            .mouseLeftPressed = mouseDown && !lastMouseDown,
            .mouseLeftReleased = !mouseDown && lastMouseDown
        };
        lastMouseDown = mouseDown;

    	// ----------- PAUSE MENU ---------------
		static bool isPaused = false;
    	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    		lastEscPress = currentTime;
    		if (escDeltaTime > menuCoolDown && game.state != MENU) isPaused = !isPaused;
    	}

    	if (isPaused) {
    		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    		ImGui::Begin("Paused", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    		if (ImGui::Button("Resume", ImVec2(160, 30))) {
    			isPaused = false;
    		}
    		if (ImGui::Button("Restart", ImVec2(160, 30))) {
    			game.state = NEWGAME;
    			isPaused = false;
    		}
    		if (ImGui::Button("Main Menu", ImVec2(160, 30))) {
    			game.state = MENU;
    			isPaused = false;
    		}
    		ImGui::End();
    	}

    	// ---------- YOU WIN SCREEN ------------
		if (game.hasWon) {
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::Begin("You win!!!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

			if (ImGui::Button("Play Again", ImVec2(140, 25))) {
				game.state = NEWGAME;
				game.hasWon = false;
			}
			if (ImGui::Button("Main Menu", ImVec2(140, 25))) {
				game.state = MENU;
				game.hasWon = false;
			}
			ImGui::End();
		}

        // ---------- STATE: NEW GAME -----------
        if (game.state == NEWGAME) {
            game.startNewGame(difficulty);
        }

        // ----------- STATE: MENU ------------
        if (game.state == MENU) {
            showMenu(game.state, difficulty);
        }

        // ---------- STATE: GAME -----------
        if (game.state == GAME) {
        	if (!isPaused) {
        		game.update(deltaTime, input.mouseWorld, input.mouseDown);
        	}
        	renderer.beginFrame(window_width, window_height);
        	game.render(renderer);
        	Renderer::endFrame();

        } else {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        ImGui::Render();
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