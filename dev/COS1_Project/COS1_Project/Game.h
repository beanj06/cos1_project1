#pragma once
#include <string>

#include "Input.h"
#include "Pet.h"
#include "Terminal.h"

class Game {
public:
    void run();  // runs until the player quits or presses Ctrl+C

private:
    void handleInput();
    void update(double dt);  // dt = seconds since the last update
    void render();
    void showMessage(const std::string& text);
    void drawStat(int row, const std::string& label, const Stat& stat);

    // Terminal and Input restore the user's terminal when the Game is destroyed.
    Terminal terminal_;
    Input input_;
    Pet pet_;

    bool running_ = true;
    std::string message_;
    double messageTimer_ = 0.0;  // seconds left to show the message
};