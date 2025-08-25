#include "MenuManager.h"
#include <stdlib.h>
#include <iostream>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

void MenuManager::Draw() {
    updateMusic();

    // Einheitlicher halbtransparenter Hintergrund
drawBackgroundOverlay(IM_COL32(0, 0, 0, 110));

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

void MenuManager::drawBackgroundOverlay(ImU32 color) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    drawList->AddRectFilled(
        viewport->Pos,
        ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y),
        color
    );
}




void MenuManager::drawStart() {
    centerWindowStart("Willkommen zum Spiel!");

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));          

    if (ImGui::Button("Leicht", ImVec2(200, 40))) {
        difficulty = Difficulty::Easy;
        resetRequested = true;
        state = MenuState::Playing;
    }

    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::Button("Schwer", ImVec2(200, 40))) {
        difficulty = Difficulty::Hard;
        resetRequested = true;
        state = MenuState::Playing;
    }

    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::Button("Multiplayer", ImVec2(200, 40))) {
        difficulty = Difficulty::Hard;
        resetRequested = true;
        state = MenuState::MultiPlayer;
    }

    ImGui::PopStyleColor(4);

    ImGui::EndGroup();
    ImGui::End();
}

void MenuManager::drawGameOver() {
    centerWindowStart("Glueckwunsch!");

    if (ImGui::Button("Neustarten", ImVec2(200, 40))) {
        state = (lastMode == MenuState::MultiPlayer) ? MenuState::MultiPlayer : MenuState::Playing;
        resetRequested = true;
    }

    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::Button("Zurück zum Menü", ImVec2(200, 40))) {
        state = MenuState::Start;
    }

    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::Button("Beenden", ImVec2(200, 40))) {
        exit(0);
    }

    ImGui::EndGroup();
    ImGui::End();
}

void MenuManager::centerWindowStart(const char* title) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::Begin("UI", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoBackground);

    // Nur Buttons zentrieren
    ImGui::SetCursorPosY(viewport->Size.y / 2.0f - 50);
    ImGui::SetCursorPosX((viewport->Size.x - 200.0f) * 0.5f);
    ImGui::BeginGroup();

    // Schrift separat (ohne Einfluss auf Layout)
    DrawWaveText(title);
}



void MenuManager::DrawWaveText(const char* text) {
    float time = ImGui::GetTime();
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Font aktivieren
    if (ImGui::GetIO().Fonts->Fonts.Size > 1)
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

    // Position berechnen (zentriert oben)
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float startX = viewport->Pos.x + (viewport->Size.x - textSize.x) * 0.5f;
    float baseY = viewport->Pos.y + viewport->Size.y / 2.0f - 120;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList(); // unabhängig vom Layout

    float x = startX;
    for (int i = 0; text[i] != '\0'; ++i) {
        float offsetY = sinf(time * 2.0f + i * 0.5f) * 5.0f;
        drawList->AddText(ImVec2(x, baseY + offsetY), IM_COL32(255, 255, 255, 255), std::string(1, text[i]).c_str());
        x += ImGui::CalcTextSize(std::string(1, text[i]).c_str()).x;
    }

    if (ImGui::GetIO().Fonts->Fonts.Size > 1)
        ImGui::PopFont();
}








void MenuManager::updateMusic() {
    if (!musicInitialized) {
        ma_engine_init(NULL, &engine);
        musicInitialized = true;
    }

    const char* track = nullptr;

    switch (state) {
    case MenuState::Start:
    case MenuState::GameWon:
        track = "menu_music.mp3";
        break;
    case MenuState::Playing:
    case MenuState::MultiPlayer:
        if (!audioReadyForGame) return;
        track = "game_music.mp3";
        break;
    default:
        break;
    }

    if (track && loadedTrack != track) {
        ma_sound_uninit(&currentMusic);

        if (ma_sound_init_from_file(&engine, track, MA_SOUND_FLAG_STREAM, NULL, NULL, &currentMusic) == MA_SUCCESS) {
            ma_sound_set_looping(&currentMusic, MA_TRUE);
            ma_sound_start(&currentMusic);
            loadedTrack = track;
            std::cout << "Musik gestartet: " << track << "\n";
        }
        else {
            std::cerr << "Musik konnte nicht geladen werden: " << track << "\n";
        }
    }
}
