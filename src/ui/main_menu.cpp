#include "main_menu.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
