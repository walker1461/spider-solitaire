#include "pause_menu.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void showPauseMenu(GameState& gameState, bool& isPaused) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Paused", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Resume", ImVec2(160, 30))) {
        isPaused = false;
    }
    if (ImGui::Button("Restart", ImVec2(160, 30))) {
        gameState = NEWGAME;
        isPaused = false;
    }
    if (ImGui::Button("Main Menu", ImVec2(160, 30))) {
        gameState = MENU;
        isPaused = false;
    }
    ImGui::End();
};

void drawPauseBackground() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.2f));
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);

    ImGui::Begin("##overlay", nullptr,
    ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoInputs |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNav);

    ImGui::GetWindowDrawList()->AddRectFilled(vp->Pos, ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y),
        IM_COL32(0, 0, 0, 40));
    ImGui::End();
    ImGui::PopStyleColor();
};