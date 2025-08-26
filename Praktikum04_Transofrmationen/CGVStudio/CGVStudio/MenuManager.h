#pragma once
#include "imgui.h"
#include <string>
#include "miniaudio.h"

enum class MenuState {
    Start,
    Playing,
    GameWon,
    MultiPlayer,
    Loading,
    LoadingStarted
};

enum class Difficulty {
    Easy,
    Hard
};



class MenuManager {
public:
    MenuState state = MenuState::Start;
    Difficulty difficulty = Difficulty::Easy; 
    void Draw();
    bool wantsToStartGame() const { return startGame; }
    void resetStartGameFlag() { startGame = false; }
    static MenuManager& instance() {
        static MenuManager mgr;
        return mgr;
    }
    bool resetRequested = false;
    MenuState lastMode = MenuState::Playing;
    bool startGame = false;
    void updateMusic();
    bool audioReadyForGame = false;
    void DrawWaveText(const char* text);
    bool loadingTriggered = false;
    bool multiplayerSelected = false;



private:
    void drawStart();
    void drawGameOver();
    void drawLoading();

    ma_engine engine;
    ma_sound currentMusic;
    std::string loadedTrack = "";
    bool musicInitialized = false;

    void drawBackgroundOverlay(ImU32 color);
    void centerWindowStart(const char* title);
 
};
