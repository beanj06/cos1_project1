#include "Game.h"

#include <chrono>
#include <csignal>
#include <thread>

// Ctrl+C just sets this flag, so the loop can end normally
// and the terminal gets restored.
static volatile std::sig_atomic_t g_interrupted = 0;

static void onSignal(int) { g_interrupted = 1; }

void Game::run() {
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    auto last = std::chrono::steady_clock::now();

    while (running_ && !g_interrupted) {
        // delta time: how many seconds passed since the previous frame
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration<double>(now - last).count();
        last = now;

        handleInput();
        update(dt);
        render();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // ~20 frames/second
    }
}

void Game::handleInput() {
    KeyEvent e = input_.poll();
    while (e.key != Key::None) {
        if (e.key == Key::Escape) running_ = false;

        if (e.key == Key::Char) {
            // Temporary test controls. These get replaced by the real menu later.
            switch (e.ch) {
            case 'q':
                running_ = false;
                break;
            case 'f':
                pet_.hunger().add(20);
                showMessage(pet_.name() + " munches happily.");
                break;
            case 'p':
                pet_.happiness().add(15);
                pet_.energy().add(-10);
                showMessage(pet_.name() + " loves playing!");
                break;
            case 's':
                pet_.energy().add(25);
                showMessage(pet_.name() + " takes a nap.");
                break;
            case 'm':
                pet_.health().add(20);
                showMessage(pet_.name() + " feels better.");
                break;
            }
        }

        e = input_.poll();
    }
}

void Game::update(double dt) {
    pet_.update(dt);

    if (messageTimer_ > 0.0) {
        messageTimer_ -= dt;
        if (messageTimer_ <= 0.0) message_ = "";
    }
}

void Game::showMessage(const std::string& text) {
    message_ = text;
    messageTimer_ = 2.5;
}

void Game::drawStat(int row, const std::string& label, const Stat& stat) {
    terminal_.writeAt(row, 2, label);
    terminal_.drawBar(row, 14, 20, stat.ratio());
    terminal_.writeAt(row, 37, std::to_string(static_cast<int>(stat.value())) + "%");
}

void Game::render() {
    terminal_.beginFrame();

    terminal_.writeAt(0, 2, "=== TERMINAL PET ===", Color::Cyan);
    terminal_.writeAt(2, 2, pet_.name() + "   age " + pet_.formattedAge(), Color::White);

    // Placeholder pet art (real animation comes in step 6)
    terminal_.writeAt(4, 4, "/\\_/\\", Color::Yellow);
    terminal_.writeAt(5, 3, "( o.o )", Color::Yellow);
    terminal_.writeAt(6, 4, "> ^ <", Color::Yellow);

    drawStat(8, "Hunger", pet_.hunger());
    drawStat(9, "Happiness", pet_.happiness());
    drawStat(10, "Energy", pet_.energy());
    drawStat(11, "Health", pet_.health());

    terminal_.writeAt(13, 2, message_, Color::Yellow);
    terminal_.writeAt(15, 2, "[F]eed  [P]lay  [S]leep  [M]edicine  [Q]uit");

    terminal_.endFrame();
}