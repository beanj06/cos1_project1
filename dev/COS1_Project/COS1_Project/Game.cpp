#include "Game.h"

#include <cctype>
#include <chrono>
#include <csignal>
#include <thread>

#include "Config.h"
#include "Save.h"

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

// ------------------------------------------------------------------ start / loop

Game::Game() {
    LoadResult result = SaveGame::load(pet_, Config::kSaveFile);

    if (result.status == LoadStatus::Ok && pet_.isAlive()) {
        log("Welcome back, " + pet_.name() + "!");
        return;
    }

    // No save, a broken save, or a save of a pet that already died: start fresh.
    if (result.status == LoadStatus::Corrupt) {
        log("Save file was unreadable. Started over.");
    }
    else if (result.status == LoadStatus::Ok) {
        SaveGame::erase(Config::kSaveFile);
    }
    pet_ = Pet();
    log("You hatched " + pet_.name() + "!");
}

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

    // Quit (q, Esc or Ctrl+C): save on the way out.
    if (screen_ == Screen::Playing) saveNow();
}

// ------------------------------------------------------------------ input

void Game::handleInput() {
    KeyEvent e = input_.poll();
    while (e.key != Key::None) {
        if (screen_ == Screen::GameOver) handleGameOverKey(e);
        else handlePlayingKey(e);
        e = input_.poll();
    }
}

void Game::handlePlayingKey(const KeyEvent& e) {
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
            return;
        }
        for (int i = 0; i < kMenuSize; ++i) {
            if (kMenu[i].hotkey == c) {
                selected_ = i;
                useAction(i);
            }
        }
    }
}

void Game::handleGameOverKey(const KeyEvent& e) {
    if (e.key == Key::Escape) {
        running_ = false;
    }
    else if (e.key == Key::Char) {
        char c = static_cast<char>(std::tolower(static_cast<unsigned char>(e.ch)));
        if (c == 'q') running_ = false;
        if (c == 'r') restart();
    }
}

void Game::useAction(int menuIndex) {
    ActionResult result = pet_.perform(kMenu[menuIndex].action);
    log(result.message);
}

// ------------------------------------------------------------------ update

void Game::update(double dt) {
    animator_.update(dt, pet_.state());
    if (savedFlash_ > 0.0) savedFlash_ -= dt;

    if (screen_ == Screen::GameOver) return;

    pet_.update(dt);
    for (PetEvent e : pet_.takeEvents()) handleEvent(e);

    // Show the dead pet for a few seconds, then the game over screen.
    if (!pet_.isAlive()) {
        deathTimer_ += dt;
        if (deathTimer_ >= Config::kGameOverDelay) screen_ = Screen::GameOver;
        return;
    }

    warnIfLow(0, pet_.hunger(), " is hungry!");
    warnIfLow(1, pet_.happiness(), " is sad.");
    warnIfLow(2, pet_.energy(), " is tired.");
    warnIfLow(3, pet_.cleanliness(), " is dirty!");
    warnIfLow(4, pet_.health(), " is feeling sick!");

    saveTimer_ += dt;
    if (saveTimer_ >= Config::kAutosaveInterval) saveNow();
}

// Turns things that happened to the pet into messages for the player.
void Game::handleEvent(PetEvent e) {
    const std::string& n = pet_.name();
    switch (e) {
    case PetEvent::WokeUp:       log(n + " wakes up, fully rested."); break;
    case PetEvent::GotSick:      log(n + " got sick! Give it medicine."); break;
    case PetEvent::WantsToPlay:  log(n + " wants to play!"); break;
    case PetEvent::PlayIgnored:  log(n + " got bored of waiting."); break;
    case PetEvent::MadeMess:     log(n + " made a mess!"); break;
    case PetEvent::Died:
        log(n + " died of " + pet_.deathCauseText() + ".");
        SaveGame::erase(Config::kSaveFile);  // no loading a dead pet
        break;
    }
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

void Game::restart() {
    std::string name = pet_.name();  // same name, new pet
    pet_ = Pet(name);
    animator_ = Animator();

    log_.clear();
    for (bool& warned : lowWarned_) warned = false;
    selected_ = 0;
    deathTimer_ = 0.0;
    saveTimer_ = 0.0;
    screen_ = Screen::Playing;

    log("You hatched " + name + "!");
    saveNow();
}

void Game::saveNow() {
    saveTimer_ = 0.0;
    if (!pet_.isAlive()) return;
    if (SaveGame::save(pet_, Config::kSaveFile)) savedFlash_ = 2.0;
    else log("Couldn't save the game!");
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

    switch (pet_.state()) {
    case PetState::Dead:       color = Color::Red;     return n + " has passed away.";
    case PetState::Sleeping:   color = Color::Blue;    return n + " is sleeping. Zzz...";
    case PetState::Eating:     color = Color::Yellow;  return n + " is eating.";
    case PetState::Playing:    color = Color::Green;   return n + " is playing!";
    case PetState::Cleaning:   color = Color::Cyan;    return n + " is getting scrubbed.";
    case PetState::Medicating: color = Color::Magenta; return n + " is taking medicine.";
    case PetState::Sick:
        color = Color::Red;
        return pet_.isSick() ? n + " is sick! Needs medicine." : n + " is in bad shape!";
    case PetState::Hungry:     color = Color::Red;     return n + " is hungry.";
    case PetState::Tired:      color = Color::Red;     return n + " is exhausted.";
    case PetState::Dirty:      color = Color::Red;     return n + " is filthy.";
    case PetState::Sad:        color = Color::Red;     return n + " is miserable.";
    case PetState::Happy:      color = Color::Green;   return n + " is happy!";
    case PetState::Content:    color = Color::White;   return n + " is doing okay.";
    }
    color = Color::White;
    return "";
}

void Game::drawPet(int row, int col) {
    const ArtFrame& frame = animator_.frame();
    for (size_t i = 0; i < frame.lines.size(); ++i) {
        terminal_.writeAt(row + static_cast<int>(i), col, frame.lines[i], animator_.color());
    }
}

void Game::drawStat(int row, const std::string& label, const Stat& stat) {
    terminal_.writeAt(row, 24, label);
    terminal_.drawBar(row, 36, 20, stat.ratio());
    terminal_.writeAt(row, 59, std::to_string(static_cast<int>(stat.value())) + "%");
}

void Game::render() {
    terminal_.beginFrame();
    if (screen_ == Screen::GameOver) renderGameOver();
    else renderPlaying();
    terminal_.endFrame();
}

void Game::renderPlaying() {
    Terminal& t = terminal_;

    // --- header ---
    t.writeAt(0, 2, "=== TERMINAL PET ===", Color::Cyan);
    t.writeAt(2, 2, pet_.name() + "   age " + pet_.formattedAge(), Color::White);

    // --- animated pet ---
    drawPet(3, 3);

    // --- stat bars ---
    drawStat(4, "Hunger", pet_.hunger());
    drawStat(5, "Happiness", pet_.happiness());
    drawStat(6, "Energy", pet_.energy());
    drawStat(7, "Cleanliness", pet_.cleanliness());
    drawStat(8, "Health", pet_.health());

    // --- status line (+ a reminder if it's asking for something) ---
    Color statusColor = Color::White;
    std::string status = statusLine(statusColor);
    t.writeAt(10, 2, status, statusColor);
    if (pet_.wantsToPlay() && pet_.isAlive()) {
        int seconds = static_cast<int>(pet_.playRequestLeft()) + 1;
        t.writeAt(11, 2, "(!) " + pet_.name() + " wants to play! (" + std::to_string(seconds) + "s)",
            Color::Yellow);
    }

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
    t.writeAt(19, 2, "Up/Down: select    Enter: use    Q: quit (saves)");
    if (savedFlash_ > 0.0) t.writeAt(19, 62, "[saved]", Color::Green);
}

void Game::renderGameOver() {
    Terminal& t = terminal_;

    t.writeAt(0, 2, "=== TERMINAL PET ===", Color::Cyan);
    t.writeAt(2, 31, "G A M E   O V E R", Color::Red);

    drawPet(4, 33);

    t.writeAt(9, 2, pet_.name() + " died of " + pet_.deathCauseText() + ".", Color::White);
    t.writeAt(10, 2, "It lived for " + pet_.formattedAge() + ".", Color::White);

    t.writeAt(12, 2, "Final stats:", Color::Cyan);
    drawStat(13, "Hunger", pet_.hunger());
    drawStat(14, "Happiness", pet_.happiness());
    drawStat(15, "Energy", pet_.energy());
    drawStat(16, "Cleanliness", pet_.cleanliness());
    drawStat(17, "Health", pet_.health());

    t.writeAt(20, 2, "[R] Hatch a new pet      [Q] Quit", Color::Yellow);
}