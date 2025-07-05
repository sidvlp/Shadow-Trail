#pragma once
#include "imgui.h"
enum class MenuState {
    Start,
    Playing,
    GameWon,
    MultiPlayer
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
private:
    void drawStart();
    void drawGameOver();
 
};
