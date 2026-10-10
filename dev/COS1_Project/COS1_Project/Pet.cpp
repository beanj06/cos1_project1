#include "Pet.h"

#include <algorithm>
#include <cstdio>
#include <utility>

Stat::Stat(float initial, float min, float max) : value_(initial), min_(min), max_(max) {
    set(initial);  // clamp the starting value too
}

float Stat::ratio() const {
    if (max_ <= min_) return 0.0f;
    return (value_ - min_) / (max_ - min_);
}

void Stat::set(float v) { value_ = std::clamp(v, min_, max_); }

void Stat::add(float delta) { set(value_ + delta); }

Pet::Pet(std::string name)
    : name_(std::move(name)),
    hunger_(70.0f),
    happiness_(70.0f),
    energy_(70.0f),
    health_(100.0f) {}

std::string Pet::formattedAge() const {
    const long total = static_cast<long>(ageSeconds_);
    char buf[32];
    std::snprintf(buf, sizeof buf, "%ldm %02lds", total / 60, total % 60);
    return buf;
}

void Pet::update(double dt) {
    if (!isAlive()) return;
    ageSeconds_ += dt;
}
