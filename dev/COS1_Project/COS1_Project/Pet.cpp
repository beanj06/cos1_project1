#include "Pet.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "Config.h"

// ---------------------------------------------------------------------- Stat
Stat::Stat(float initial, float min, float max) : value_(initial), min_(min), max_(max) {
    set(initial);  // clamp the starting value too
}

float Stat::ratio() const {
    if (max_ <= min_) return 0.0f;
    return (value_ - min_) / (max_ - min_);
}

void Stat::set(float v) { value_ = std::clamp(v, min_, max_); }

void Stat::add(float delta) { set(value_ + delta); }

// ------------------------------------------------------------------ Pet

Pet::Pet(std::string name)
    : name_(std::move(name)),
    hunger_(70.0f),
    happiness_(70.0f),
    energy_(70.0f),
    cleanliness_(100.0f),
    health_(100.0f) {}

std::string Pet::formattedAge() const {
    const long total = static_cast<long>(ageSeconds_);
    char buf[32];
    std::snprintf(buf, sizeof buf, "%ldm %02lds", total / 60, total % 60);
    return buf;
}

float Pet::cooldownLeft(Action action) const {
    return cooldown_[static_cast<int>(action)];
}

ActionResult Pet::perform(Action action) {
    if (!isAlive()) return { false, name_ + " is no longer with us." };

    // While asleep, the only thing you can do is wake up.
    if (sleeping_) {
        if (action == Action::Sleep) {
            sleeping_ = false;
            cooldown_[static_cast<int>(Action::Sleep)] = Config::kSleepCooldown;
            return { true, name_ + " wakes up." };
        }
        return { false, name_ + " is asleep. Wake up first." };
    }

    float& cooldown = cooldown_[static_cast<int>(action)];
    if (cooldown > 0.0f) {
        int seconds = static_cast<int>(std::ceil(cooldown));
        return { false, "Not ready yet (" + std::to_string(seconds) + "s)." };
    }

    switch (action) {
    case Action::Feed:
        if (hunger_.value() > Config::kFullAbove) return { false, name_ + " isn't hungry." };
        hunger_.add(Config::kFeedHunger);
        happiness_.add(Config::kFeedHappiness);
        cleanliness_.add(-Config::kFeedMess);
        cooldown = Config::kFeedCooldown;
        return { true, name_ + " munches happily." };

    case Action::Play:
        if (energy_.value() < Config::kPlayMinEnergy) {
            return { false, name_ + " is too tired to play." };
        }
        happiness_.add(Config::kPlayHappiness);
        energy_.add(-Config::kPlayEnergy);
        hunger_.add(-Config::kPlayHunger);
        cleanliness_.add(-Config::kPlayMess);
        cooldown = Config::kPlayCooldown;
        return { true, name_ + " has fun playing!" };

    case Action::Sleep:
        if (energy_.value() > Config::kRestedAbove) return { false, name_ + " isn't tired." };
        sleeping_ = true;
        return { true, "Lights off. " + name_ + " falls asleep." };

    case Action::Clean:
        if (cleanliness_.value() > Config::kCleanAbove) return { false, name_ + " is already clean." };
        cleanliness_.add(Config::kCleanAmount);
        cooldown = Config::kCleanCooldown;
        return { true, name_ + " gets a nice scrub." };

    case Action::Medicine:
        if (health_.value() > Config::kHealthyAbove) return { false, name_ + " doesn't need medicine." };
        health_.add(Config::kMedicineHealth);
        happiness_.add(-Config::kMedicineHappiness);
        cooldown = Config::kMedicineCooldown;
        return { true, name_ + " takes the medicine. Yuck." };
    }
    return { false, "" };
}

void Pet::update(double dt) {
    if (!isAlive()) return;

    const float d = static_cast<float>(dt) * Config::kTimeScale;
    ageSeconds_ += d;

    for (float& c : cooldown_) {
        if (c > 0.0f) c = std::max(0.0f, c - d);
    }

    // Everything drops more slowly while sleeping.
    const float factor = sleeping_ ? Config::kSleepDecayFactor : 1.0f;
    hunger_.add(-Config::kHungerDecay * factor * d);
    happiness_.add(-Config::kHappinessDecay * factor * d);
    cleanliness_.add(-Config::kCleanlinessDecay * factor * d);

    if (sleeping_) {
        energy_.add(Config::kSleepEnergyGain * d);
        if (energy_.isFull()) {  // fully rested: wake up on its own
            sleeping_ = false;
            cooldown_[static_cast<int>(Action::Sleep)] = Config::kSleepCooldown;
        }
    }
    else {
        energy_.add(-Config::kEnergyDecay * d);
    }

    // Health: drops for every stat that hit 0, slowly recovers otherwise.
    int empty = 0;
    if (hunger_.isEmpty()) ++empty;
    if (energy_.isEmpty()) ++empty;
    if (cleanliness_.isEmpty()) ++empty;

    if (empty > 0) {
        health_.add(-Config::kHealthLoss * static_cast<float>(empty) * d);
    }
    else if (hunger_.value() > Config::kRegenMinStat &&
        energy_.value() > Config::kRegenMinStat &&
        cleanliness_.value() > Config::kRegenMinStat) {
        health_.add(Config::kHealthRegen * d);
    }
}