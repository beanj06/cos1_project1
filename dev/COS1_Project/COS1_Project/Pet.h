#pragma once
#include <random>
#include <string>
#include <utility>
#include <vector>

// A value clamped to [min, max]. Every change goes through set()/add(),
// so it can never go out of range.
class Stat {
public:
    explicit Stat(float initial = 100.0f, float min = 0.0f, float max = 100.0f);

    float value() const { return value_; }
    float min() const { return min_; }
    float max() const { return max_; }
    float ratio() const;  // 0.0 - 1.0, handy for drawing bars

    void set(float v);
    void add(float delta);  // negative delta decreases

    bool isEmpty() const { return value_ <= min_; }
    bool isFull() const { return value_ >= max_; }

private:
    float value_;
    float min_;
    float max_;
};

enum class Action { Feed, Play, Sleep, Clean, Medicine };
constexpr int kActionCount = 5;

// What happened when you tried an action.
struct ActionResult {
    bool success;
    std::string message;
};

// The pet's current state. It is worked out from the stats every time you
// ask (see Pet::state()), so it can never get out of sync with them.
// The last four are short "activity" states shown right after an action.
enum class PetState {
    Happy, Content, Hungry, Tired, Dirty, Sad, Sick, Sleeping, Dead,
    Eating, Playing, Cleaning, Medicating
};

// Things that happen on their own. The Game collects them with
// Pet::takeEvents() and decides what to tell the player.
enum class PetEvent { WokeUp, Died, GotSick, WantsToPlay, PlayIgnored, MadeMess };

enum class DeathCause { None, Starvation, Exhaustion, Filth, Illness };

// For every stat, HIGHER IS BETTER: hunger 100 = full belly, 0 = starving.
class Pet {
public:
    explicit Pet(std::string name = "Pip");

    const std::string& name() const { return name_; }
    void setName(std::string name) { name_ = std::move(name); }

    Stat& hunger() { return hunger_; }
    Stat& happiness() { return happiness_; }
    Stat& energy() { return energy_; }
    Stat& cleanliness() { return cleanliness_; }
    Stat& health() { return health_; }
    const Stat& hunger() const { return hunger_; }
    const Stat& happiness() const { return happiness_; }
    const Stat& energy() const { return energy_; }
    const Stat& cleanliness() const { return cleanliness_; }
    const Stat& health() const { return health_; }

    double ageSeconds() const { return ageSeconds_; }
    void setAgeSeconds(double s) { ageSeconds_ = s < 0 ? 0 : s; }
    std::string formattedAge() const;  // e.g. "3m 07s"

    bool isAlive() const { return !health_.isEmpty(); }
    bool isSleeping() const { return sleeping_; }
    bool isSick() const { return sick_; }
    bool wantsToPlay() const { return wantsPlay_; }
    float playRequestLeft() const { return playRequestTimer_; }  // seconds

    PetState state() const;

    DeathCause deathCause() const { return deathCause_; }
    std::string deathCauseText() const;  // "starvation", "illness", ...

    // Seconds until the action can be used again (0 = ready).
    float cooldownLeft(Action action) const;

    // Tries to do an action. Check result.success / result.message.
    ActionResult perform(Action action);

    // Advances time by dt seconds: stats decay, cooldowns tick down,
    // random events may happen.
    void update(double dt);

    // Returns everything that happened since the last call, then forgets it.
    std::vector<PetEvent> takeEvents();

private:
    friend class SaveGame;  // the save system reads/writes the private fields

    enum class Activity { None, Eating, Playing, Cleaning, Medicating };

    void startActivity(Activity activity);
    void triggerRandomEvent();
    float randomRange(float low, float high);

    std::string name_;
    Stat hunger_;
    Stat happiness_;
    Stat energy_;
    Stat cleanliness_;
    Stat health_;
    double ageSeconds_ = 0.0;

    bool sleeping_ = false;
    bool sick_ = false;
    bool wantsPlay_ = false;
    float playRequestTimer_ = 0.0f;
    float cooldown_[kActionCount] = {};
    float nextEventIn_ = 60.0f;

    Activity activity_ = Activity::None;
    float activityTimer_ = 0.0f;

    DeathCause deathCause_ = DeathCause::None;
    std::vector<PetEvent> events_;
    std::mt19937 rng_;
};