#include "MenuManager.h"
#include <stdlib.h>

void MenuManager::Draw() {
    switch (state) {
    case MenuState::Start:
        drawStart();
        break;
    case MenuState::GameWon:
        drawGameOver();
        break;
    default:
        break;
    }
}

void MenuManager::drawStart() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::Begin("Startmenü", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoBackground);

    // Halbtransparenter Hintergrund
    ImGui::GetWindowDrawList()->AddRectFilled(
        viewport->Pos,
        ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y),
        IM_COL32(0, 0, 0, 200)
    );

    // Zentrierte Buttons
    ImGui::SetCursorPosY(viewport->Size.y / 2.0f - 80);
    ImGui::SetCursorPosX(viewport->Size.x / 2.0f - 100);
    ImGui::BeginGroup();

    ImGui::Text("Willkommen zum Spiel!");

    if (ImGui::Button("Leicht", ImVec2(200, 40))) {
        difficulty = Difficulty::Easy;
        state = MenuState::Playing;
    }

    ImGui::Dummy(ImVec2(0, 10)); // Kleiner Abstand zwischen den Buttons


    if (ImGui::Button("Schwer", ImVec2(200, 40))) {
        difficulty = Difficulty::Hard;
        state = MenuState::Playing;
    }

    ImGui::Dummy(ImVec2(0, 10)); // Kleiner Abstand zwischen den Buttons

    if (ImGui::Button("Multiplayer", ImVec2(200, 40))) {
        difficulty = Difficulty::Hard;
        state = MenuState::MultiPlayer;
    }


    ImGui::EndGroup();
    ImGui::End();
}



void MenuManager::drawGameOver() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::Begin("Spiel beendet", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoBackground);

    ImGui::GetWindowDrawList()->AddRectFilled(
        viewport->Pos,
        ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y),
        IM_COL32(0, 0, 0, 200)
    );

    ImGui::SetCursorPosY(viewport->Size.y / 2.0f - 60);
    ImGui::SetCursorPosX(viewport->Size.x / 2.0f - 100);
    ImGui::BeginGroup();
    ImGui::Text("Glueckwunsch!");
    if (ImGui::Button("Neustarten", ImVec2(200, 40))) {
      
        state = (lastMode == MenuState::MultiPlayer) ? MenuState::MultiPlayer : MenuState::Playing;
        resetRequested = true;
    }

    ImGui::Dummy(ImVec2(0, 10)); // Kleiner Abstand zwischen den Buttons

    if (ImGui::Button("Zurück zum Menü", ImVec2(200, 40))) {
        state = MenuState::Start;
    }

    ImGui::Dummy(ImVec2(0, 10)); // Kleiner Abstand zwischen den Buttons


    if (ImGui::Button("Beenden", ImVec2(200, 40))) {
        exit(0);
    }
    ImGui::EndGroup();

    ImGui::End();
}
