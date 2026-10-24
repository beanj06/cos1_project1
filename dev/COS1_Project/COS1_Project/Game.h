#pragma once
#include <string>
#include <vector>

#include "Art.h"
#include "Input.h"
#include "Pet.h"
#include "Terminal.h"

class Game {
public:
    Game();      // loads the save file if there is one
    void run();  // runs until the player quits or presses Ctrl+C

private:
    enum class Screen { Playing, GameOver };

    // input
    void handleInput();
    void handlePlayingKey(const KeyEvent& e);
    void handleGameOverKey(const KeyEvent& e);
    void useAction(int menuIndex);

    // update
    void update(double dt);
    void handleEvent(PetEvent e);
    void warnIfLow(int index, const Stat& stat, const std::string& text);
    void restart();
    void saveNow();
    void log(const std::string& text);

    // drawing
    void render();
    void renderPlaying();
    void renderGameOver();
    void drawPet(int row, int col);
    void drawStat(int row, const std::string& label, const Stat& stat);
    std::string statusLine(Color& color) const;

    // Terminal and Input restore the user's terminal when the Game is destroyed.
    Terminal terminal_;
    Input input_;
    Pet pet_;
    Animator animator_;

    Screen screen_ = Screen::Playing;
    bool running_ = true;
    int selected_ = 0;                // highlighted menu item
    std::vector<std::string> log_;    // last few messages
    bool lowWarned_[5] = {};          // so "is hungry!" only shows once per dip

    double deathTimer_ = 0.0;         // how long the dead pet has been shown
    double saveTimer_ = 0.0;          // time since the last autosave
    double savedFlash_ = 0.0;         // shows "saved" for a moment after saving
};