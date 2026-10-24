#include "Pet.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>

#include "Config.h"

// ------------------------------------------------------------------ Stat

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

static unsigned makeSeed() {
    unsigned t = static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count());
    return t ^ std::random_device{}();
}

Pet::Pet(std::string name)
    : name_(std::move(name)),
    hunger_(70.0f),
    happiness_(70.0f),
    energy_(70.0f),
    cleanliness_(100.0f),
    health_(100.0f),
    rng_(makeSeed()) {
    nextEventIn_ = randomRange(Config::kEventMinDelay, Config::kEventMaxDelay);
}

float Pet::randomRange(float low, float high) {
    std::uniform_real_distribution<float> dist(low, high);
    return dist(rng_);
}

std::string Pet::formattedAge() const {
    const long total = static_cast<long>(ageSeconds_);
    char buf[32];
    std::snprintf(buf, sizeof buf, "%ldm %02lds", total / 60, total % 60);
    return buf;
}

// ------------------------------------------------------------------ state machine

// The one place that decides what state the pet is in.
// The checks are in priority order: the first one that matches wins.
PetState Pet::state() const {
    if (!isAlive()) return PetState::Dead;
    if (sleeping_) return PetState::Sleeping;

    // Just did something? Show that for a moment.
    if (activityTimer_ > 0.0f) {
        switch (activity_) {
        case Activity::Eating:     return PetState::Eating;
        case Activity::Playing:    return PetState::Playing;
        case Activity::Cleaning:   return PetState::Cleaning;
        case Activity::Medicating: return PetState::Medicating;
        case Activity::None:       break;
        }
    }

    if (sick_ || health_.value() < Config::kLowStat) return PetState::Sick;
    if (hunger_.value() < Config::kLowStat)          return PetState::Hungry;
    if (energy_.value() < Config::kLowStat)          return PetState::Tired;
    if (cleanliness_.value() < Config::kLowStat)     return PetState::Dirty;
    if (happiness_.value() < Config::kLowStat)       return PetState::Sad;

    if (hunger_.value() > Config::kHappyAbove && happiness_.value() > Config::kHappyAbove &&
        energy_.value() > Config::kHappyAbove && cleanliness_.value() > Config::kHappyAbove) {
        return PetState::Happy;
    }
    return PetState::Content;
}

std::string Pet::deathCauseText() const {
    switch (deathCause_) {
    case DeathCause::Starvation:  return "starvation";
    case DeathCause::Exhaustion:  return "exhaustion";
    case DeathCause::Filth:       return "filth";
    case DeathCause::Illness:     return "illness";
    case DeathCause::None:        break;
    }
    return "unknown causes";
}

// ------------------------------------------------------------------ actions

float Pet::cooldownLeft(Action action) const {
    return cooldown_[static_cast<int>(action)];
}

void Pet::startActivity(Activity activity) {
    activity_ = activity;
    activityTimer_ = Config::kActivityDuration;
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
        startActivity(Activity::Eating);
        return { true, name_ + " munches happily." };

    case Action::Play: {
        if (energy_.value() < Config::kPlayMinEnergy) {
            return { false, name_ + " is too tired to play." };
        }
        happiness_.add(Config::kPlayHappiness);
        energy_.add(-Config::kPlayEnergy);
        hunger_.add(-Config::kPlayHunger);
        cleanliness_.add(-Config::kPlayMess);
        cooldown = Config::kPlayCooldown;
        startActivity(Activity::Playing);

        if (wantsPlay_) {  // it asked for this!
            wantsPlay_ = false;
            happiness_.add(Config::kPlayRequestBonus);
            return { true, name_ + " was waiting for this! Yay!" };
        }
        return { true, name_ + " has fun playing!" };
    }

    case Action::Sleep:
        if (energy_.value() > Config::kRestedAbove) return { false, name_ + " isn't tired." };
        sleeping_ = true;
        return { true, "Lights off. " + name_ + " falls asleep." };

    case Action::Clean:
        if (cleanliness_.value() > Config::kCleanAbove) return { false, name_ + " is already clean." };
        cleanliness_.add(Config::kCleanAmount);
        cooldown = Config::kCleanCooldown;
        startActivity(Activity::Cleaning);
        return { true, name_ + " gets a nice scrub." };

    case Action::Medicine: {
        if (!sick_ && health_.value() > Config::kHealthyAbove) {
            return { false, name_ + " doesn't need medicine." };
        }
        const bool wasSick = sick_;
        sick_ = false;  // medicine cures sickness
        health_.add(Config::kMedicineHealth);
        happiness_.add(-Config::kMedicineHappiness);
        cooldown = Config::kMedicineCooldown;
        startActivity(Activity::Medicating);
        if (wasSick) return { true, name_ + " is cured! Feeling better." };
        return { true, name_ + " takes the medicine. Yuck." };
    }
    }
    return { false, "" };
}

// ------------------------------------------------------------------ time passing

void Pet::update(double dt) {
    if (!isAlive()) return;

    const float real = static_cast<float>(dt);                // real seconds
    const float d = real * Config::kTimeScale;                // game seconds
    ageSeconds_ += d;

    // The "eating/playing" pose runs on real time so it always looks the same.
    if (activityTimer_ > 0.0f) {
        activityTimer_ -= real;
        if (activityTimer_ <= 0.0f) {
            activityTimer_ = 0.0f;
            activity_ = Activity::None;
        }
    }

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
            events_.push_back(PetEvent::WokeUp);
        }
    }
    else {
        energy_.add(-Config::kEnergyDecay * d);
    }

    // Sickness keeps hurting until you give medicine.
    if (sick_) {
        health_.add(-Config::kSickHealthLoss * d);
        happiness_.add(-Config::kSickHappinessLoss * d);
    }

    // Health: drops for every stat that hit 0, slowly recovers otherwise.
    int empty = 0;
    if (hunger_.isEmpty()) ++empty;
    if (energy_.isEmpty()) ++empty;
    if (cleanliness_.isEmpty()) ++empty;

    if (empty > 0) {
        health_.add(-Config::kHealthLoss * static_cast<float>(empty) * d);
    }
    else if (!sick_ &&
        hunger_.value() > Config::kRegenMinStat &&
        energy_.value() > Config::kRegenMinStat &&
        cleanliness_.value() > Config::kRegenMinStat) {
        health_.add(Config::kHealthRegen * d);
    }

    // Random events only happen while the pet is awake.
    if (!sleeping_) {
        if (wantsPlay_) {
            playRequestTimer_ -= d;
            if (playRequestTimer_ <= 0.0f) {  // you took too long
                wantsPlay_ = false;
                happiness_.add(-Config::kPlayIgnoredPenalty);
                events_.push_back(PetEvent::PlayIgnored);
            }
        }

        nextEventIn_ -= d;
        if (nextEventIn_ <= 0.0f) {
            triggerRandomEvent();
            nextEventIn_ = randomRange(Config::kEventMinDelay, Config::kEventMaxDelay);
        }
    }

    // Did that kill it?
    if (health_.isEmpty()) {
        if (hunger_.isEmpty())           deathCause_ = DeathCause::Starvation;
        else if (energy_.isEmpty())      deathCause_ = DeathCause::Exhaustion;
        else if (cleanliness_.isEmpty()) deathCause_ = DeathCause::Filth;
        else                             deathCause_ = DeathCause::Illness;
        events_.push_back(PetEvent::Died);
    }
}

// Picks one random event. Each event has a "weight": higher = more likely.
void Pet::triggerRandomEvent() {
    int sickWeight = 0;
    if (!sick_) {
        sickWeight = 2;
        if (cleanliness_.value() < 30.0f) sickWeight += 2;  // dirty pets get sick more
        if (hunger_.value() < 30.0f) sickWeight += 1;
    }
    int playWeight = (wantsPlay_ || energy_.value() < Config::kPlayMinEnergy) ? 0 : 3;
    int messWeight = 3;

    std::uniform_int_distribution<int> pick(0, sickWeight + playWeight + messWeight - 1);
    int roll = pick(rng_);

    if (roll < sickWeight) {
        sick_ = true;
        health_.add(-Config::kSickHealthHit);
        events_.push_back(PetEvent::GotSick);
        return;
    }
    roll -= sickWeight;

    if (roll < playWeight) {
        wantsPlay_ = true;
        playRequestTimer_ = Config::kPlayRequestTime;
        events_.push_back(PetEvent::WantsToPlay);
        return;
    }

    cleanliness_.add(-Config::kMessAmount);
    events_.push_back(PetEvent::MadeMess);
}

std::vector<PetEvent> Pet::takeEvents() {
    std::vector<PetEvent> result;
    result.swap(events_);
    return result;
}