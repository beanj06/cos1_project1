#pragma once
#include <string>
#include <vector>

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

    void useAction(int menuIndex);
    void log(const std::string& text);
    void warnIfLow(int index, const Stat& stat, const std::string& text);
    std::string statusLine(Color& color) const;
    void drawStat(int row, const std::string& label, const Stat& stat);

    // Terminal and Input restore the user's terminal when the Game is destroyed.
    Terminal terminal_;
    Input input_;
    Pet pet_;

    bool running_ = true;
    int selected_ = 0;                // highlighted menu item
    std::vector<std::string> log_;    // last few messages
    bool lowWarned_[5] = {};          // so "is hungry!" only shows once per dip
};