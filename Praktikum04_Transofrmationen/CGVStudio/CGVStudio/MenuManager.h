#pragma once
#include "imgui.h"
enum class MenuState {
    Start,
    Playing,
    GameWon
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

private:
    void drawStart();
    void drawGameOver();

    bool startGame = false;
};
