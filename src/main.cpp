#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/game.h"
#include "input/input_state.h"
#include "render/render.h"

#include <random>

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

int main() {

    // ---------- GET OPENGL WINDOW ----------
    if (!glfwInit()) return -1;
    const float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = openGlInit(main_scale);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) return -1;

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
	game.initHighScores();

    static auto lastTime = static_cast<float>(glfwGetTime());

    // ----------- GAME LOOP -----------
    while (!glfwWindowShouldClose(window)) {
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwPollEvents();

    	int window_width, window_height;
    	glfwGetFramebufferSize(window, &window_width, &window_height);
    	int screen_width, screen_height;
    	glfwGetWindowSize(window, &screen_width, &screen_height);

    	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT);

        if (game.state == QUIT)
            break;

    	const auto currentTime = static_cast<float>(glfwGetTime());
    	const float deltaTime = currentTime - lastTime;
    	lastTime = currentTime;

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


        game.update(deltaTime, input.mouseWorld, input.mouseDown, window);

        renderer.beginFrame(window_width, window_height);
    	renderer.render(game);
        Renderer::endFrame();

        // game score
    	if (game.state != GameState::MENU) {
    		std::string scoreText = "Score: " + std::to_string(game.score);
    		const float textWidth = ImGui::CalcTextSize(scoreText.c_str()).x;

    		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    		ImGui::SetNextWindowSize(ImVec2(textWidth + 18.0f, 20));

    		ImGui::Begin("##score", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
    		ImGui::Text("%s", scoreText.c_str());
    		ImGui::End();
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