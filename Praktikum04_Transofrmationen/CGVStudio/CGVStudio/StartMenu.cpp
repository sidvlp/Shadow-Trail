#include "StartMenu.h"
#include "imgui.h"

void StartMenu::Draw() {
    if (!show) return;

    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Startmenü", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("Willkommen zum Spiel!");

    if (ImGui::Button("Spiel starten", ImVec2(200, 40))) {
        show = false;
    }

    ImGui::End();
}
