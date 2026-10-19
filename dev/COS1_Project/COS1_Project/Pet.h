#pragma once
#include <string>
#include <utility>

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


struct ActionResult {
    bool success;
    std::string message;
};

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

    // Seconds until the action can be used again (0 = ready).
    float cooldownLeft(Action action) const;

    // Tries to do an action. Check result.success / result.message.
    ActionResult perform(Action action);

    // Advances time by dt seconds: stats decay, cooldowns tick down.
    void update(double dt);

private:
    std::string name_;
    Stat hunger_;
    Stat happiness_;
    Stat energy_;
    Stat cleanliness_;
    Stat health_;
    double ageSeconds_ = 0.0;
    bool sleeping_ = false;
    float cooldown_[kActionCount] = {};
};