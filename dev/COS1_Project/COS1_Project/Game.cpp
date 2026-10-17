#include "Game.h"

#include <cctype>
#include <chrono>
#include <csignal>
#include <thread>

// Ctrl+C just sets this flag, so the loop can end normally
// and the terminal gets restored.
static volatile std::sig_atomic_t g_interrupted = 0;

static void onSignal(int) { g_interrupted = 1; }

// ------------------------------------------------------------------ menu

struct MenuItem {
    Action action;
    const char* label;
    char hotkey;
};

static const MenuItem kMenu[] = {
    {Action::Feed,     "Feed",     'f'},
    {Action::Play,     "Play",     'p'},
    {Action::Sleep,    "Sleep",    's'},
    {Action::Clean,    "Clean",    'c'},
    {Action::Medicine, "Medicine", 'm'},
};
static const int kMenuSize = sizeof(kMenu) / sizeof(kMenu[0]);

static const size_t kMaxLogLines = 5;

// ------------------------------------------------------------------ loop

void Game::run() {
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    log("You hatched " + pet_.name() + "!");

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

// ------------------------------------------------------------------ input

void Game::handleInput() {
    KeyEvent e = input_.poll();
    while (e.key != Key::None) {
        if (e.key == Key::Escape) {
            running_ = false;
        }
        else if (e.key == Key::Up) {
            selected_ = (selected_ + kMenuSize - 1) % kMenuSize;
        }
        else if (e.key == Key::Down) {
            selected_ = (selected_ + 1) % kMenuSize;
        }
        else if (e.key == Key::Enter) {
            useAction(selected_);
        }
        else if (e.key == Key::Char) {
            char c = static_cast<char>(std::tolower(static_cast<unsigned char>(e.ch)));
            if (c == 'q') {
                running_ = false;
            }
            else {
                for (int i = 0; i < kMenuSize; ++i) {
                    if (kMenu[i].hotkey == c) {
                        selected_ = i;
                        useAction(i);
                    }
                }
            }
        }
        e = input_.poll();
    }
}

void Game::useAction(int menuIndex) {
    ActionResult result = pet_.perform(kMenu[menuIndex].action);
    log(result.message);
}

// ------------------------------------------------------------------ update

void Game::update(double dt) {
    bool wasAlive = pet_.isAlive();
    bool wasSleeping = pet_.isSleeping();

    pet_.update(dt);

    if (wasSleeping && !pet_.isSleeping() && pet_.isAlive()) {
        log(pet_.name() + " wakes up, fully rested.");
    }
    if (wasAlive && !pet_.isAlive()) {
        log(pet_.name() + " has passed away...");
        return;
    }

    warnIfLow(0, pet_.hunger(), " is hungry!");
    warnIfLow(1, pet_.happiness(), " is sad.");
    warnIfLow(2, pet_.energy(), " is tired.");
    warnIfLow(3, pet_.cleanliness(), " is dirty!");
    warnIfLow(4, pet_.health(), " is feeling sick!");
}

// Adds a log message when a stat drops below 20, then stays quiet
// until the stat has recovered above 35.
void Game::warnIfLow(int index, const Stat& stat, const std::string& text) {
    if (!lowWarned_[index] && stat.value() < 20.0f) {
        lowWarned_[index] = true;
        log(pet_.name() + text);
    }
    else if (lowWarned_[index] && stat.value() > 35.0f) {
        lowWarned_[index] = false;
    }
}

void Game::log(const std::string& text) {
    if (text.empty()) return;
    log_.push_back(text);
    if (log_.size() > kMaxLogLines) log_.erase(log_.begin());
}

// ------------------------------------------------------------------ drawing

// One line describing how the pet is doing right now.
std::string Game::statusLine(Color& color) const {
    const std::string& n = pet_.name();
    color = Color::Red;

    if (!pet_.isAlive())                    return n + " has passed away.";
    if (pet_.isSleeping()) { color = Color::Blue; return n + " is sleeping. Zzz..."; }
    if (pet_.health().value() < 25)         return n + " is very sick!";
    if (pet_.hunger().value() < 25)         return n + " is starving!";
    if (pet_.energy().value() < 25)         return n + " is exhausted.";
    if (pet_.cleanliness().value() < 25)    return n + " is filthy.";
    if (pet_.happiness().value() < 25)      return n + " is miserable.";

    if (pet_.hunger().value() > 60 && pet_.happiness().value() > 60 &&
        pet_.energy().value() > 60 && pet_.cleanliness().value() > 60) {
        color = Color::Green;
        return n + " is happy!";
    }
    color = Color::White;
    return n + " is doing okay.";
}

void Game::drawStat(int row, const std::string& label, const Stat& stat) {
    terminal_.writeAt(row, 24, label);
    terminal_.drawBar(row, 36, 20, stat.ratio());
    terminal_.writeAt(row, 59, std::to_string(static_cast<int>(stat.value())) + "%");
}

void Game::render() {
    Terminal& t = terminal_;
    t.beginFrame();

    // --- header ---
    t.writeAt(0, 2, "=== TERMINAL PET ===", Color::Cyan);
    t.writeAt(2, 2, pet_.name() + "   age " + pet_.formattedAge(), Color::White);

    // --- pet picture (placeholder until step 6) ---
    if (!pet_.isAlive()) {
        t.writeAt(4, 4, "/\\_/\\", Color::Red);
        t.writeAt(5, 3, "( x.x )", Color::Red);
        t.writeAt(6, 4, "> - <", Color::Red);
    }
    else if (pet_.isSleeping()) {
        t.writeAt(3, 9, "z Z", Color::Blue);
        t.writeAt(4, 4, "/\\_/\\", Color::Yellow);
        t.writeAt(5, 3, "( -.- )", Color::Yellow);
        t.writeAt(6, 4, "> ^ <", Color::Yellow);
    }
    else {
        t.writeAt(4, 4, "/\\_/\\", Color::Yellow);
        t.writeAt(5, 3, "( o.o )", Color::Yellow);
        t.writeAt(6, 4, "> ^ <", Color::Yellow);
    }

    // --- stat bars ---
    drawStat(4, "Hunger", pet_.hunger());
    drawStat(5, "Happiness", pet_.happiness());
    drawStat(6, "Energy", pet_.energy());
    drawStat(7, "Cleanliness", pet_.cleanliness());
    drawStat(8, "Health", pet_.health());

    // --- status line ---
    Color statusColor;
    std::string status = statusLine(statusColor);
    t.writeAt(10, 2, status, statusColor);

    // --- action menu ---
    t.writeAt(12, 2, "-- Actions --", Color::Cyan);
    for (int i = 0; i < kMenuSize; ++i) {
        const MenuItem& item = kMenu[i];
        std::string label = item.label;
        if (item.action == Action::Sleep && pet_.isSleeping()) label = "Wake up";

        bool selected = (i == selected_);
        std::string text = std::string(selected ? "> " : "  ") + label;
        t.writeAt(13 + i, 2, text, selected ? Color::Yellow : Color::Default);

        std::string hotkey = "[";
        hotkey += static_cast<char>(std::toupper(static_cast<unsigned char>(item.hotkey)));
        hotkey += "]";
        t.writeAt(13 + i, 14, hotkey, Color::Default);

        float cooldown = pet_.cooldownLeft(item.action);
        if (cooldown > 0.0f) {
            t.writeAt(13 + i, 19, std::to_string(static_cast<int>(cooldown) + 1) + "s", Color::Red);
        }
    }

    // --- message log (newest at the bottom) ---
    t.writeAt(12, 36, "-- Log --", Color::Cyan);
    for (size_t i = 0; i < log_.size(); ++i) {
        bool newest = (i + 1 == log_.size());
        t.writeAt(13 + static_cast<int>(i), 36, log_[i], newest ? Color::White : Color::Default);
    }

    // --- help ---
    t.writeAt(19, 2, "Up/Down: select    Enter: use    Q: quit");

    t.endFrame();
}