#pragma once
#include "imgui.h"
enum class MenuState {
    Start,
    Playing,
    GameWon
};

class MenuManager {
public:
    MenuState state = MenuState::Start;

    void Draw();

private:
    void drawStart();
    void drawGameOver();
};
