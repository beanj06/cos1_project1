#include "Save.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>

namespace fs = std::filesystem;

static const int kSaveVersion = 1;
static const size_t kMaxNameLength = 20;

// Names end up on the terminal, so keep only plain printable characters
// (a save file with escape codes in the name must not be able to mess with the screen).
static std::string cleanName(const std::string& raw) {
    std::string out;
    for (char c : raw) {
        unsigned char u = static_cast<unsigned char>(c);
        if (u >= 32 && u <= 126) out += c;
    }
    size_t first = out.find_first_not_of(' ');
    if (first == std::string::npos) return "Pip";
    size_t last = out.find_last_not_of(' ');
    out = out.substr(first, last - first + 1);
    if (out.size() > kMaxNameLength) out.resize(kMaxNameLength);
    return out;
}

// ------------------------------------------------------------------ save

bool SaveGame::save(const Pet& pet, const std::string& path) {
    const std::string tempPath = path + ".tmp";
    bool ok = false;

    {
        std::ofstream out(tempPath, std::ios::trunc);
        if (out) {
            out << std::fixed << std::setprecision(3);
            out << "# Terminal Pet save file\n";
            out << "version=" << kSaveVersion << "\n";
            out << "saved_at=" << static_cast<long long>(std::time(nullptr)) << "\n";
            out << "name=" << cleanName(pet.name_) << "\n";
            out << "age=" << pet.ageSeconds_ << "\n";
            out << "hunger=" << pet.hunger_.value() << "\n";
            out << "happiness=" << pet.happiness_.value() << "\n";
            out << "energy=" << pet.energy_.value() << "\n";
            out << "cleanliness=" << pet.cleanliness_.value() << "\n";
            out << "health=" << pet.health_.value() << "\n";
            out << "sleeping=" << (pet.sleeping_ ? 1 : 0) << "\n";
            out << "sick=" << (pet.sick_ ? 1 : 0) << "\n";
            out << "wants_play=" << (pet.wantsPlay_ ? 1 : 0) << "\n";
            out << "play_request_timer=" << pet.playRequestTimer_ << "\n";
            out << "next_event_in=" << pet.nextEventIn_ << "\n";
            out << "cooldown_feed=" << pet.cooldown_[static_cast<int>(Action::Feed)] << "\n";
            out << "cooldown_play=" << pet.cooldown_[static_cast<int>(Action::Play)] << "\n";
            out << "cooldown_sleep=" << pet.cooldown_[static_cast<int>(Action::Sleep)] << "\n";
            out << "cooldown_clean=" << pet.cooldown_[static_cast<int>(Action::Clean)] << "\n";
            out << "cooldown_medicine=" << pet.cooldown_[static_cast<int>(Action::Medicine)] << "\n";
            out.flush();
            ok = static_cast<bool>(out);
        }
    }  // the file is closed here

    std::error_code ec;
    if (ok) {
        fs::rename(tempPath, path, ec);  // replaces the old save
        ok = !ec;
    }
    if (!ok) fs::remove(tempPath, ec);
    return ok;
}

// ------------------------------------------------------------------ load

using Values = std::map<std::string, std::string>;

// Reads a number from the file. A missing key is fine unless `required`.
// Text that isn't a clean, finite number makes it fail.
static bool readNumber(const Values& values, const char* key, bool required, double& out) {
    auto it = values.find(key);
    if (it == values.end()) return !required;

    const char* start = it->second.c_str();
    char* end = nullptr;
    double x = std::strtod(start, &end);
    if (end == start || *end != '\0' || !std::isfinite(x)) return false;

    out = x;
    return true;
}

LoadResult SaveGame::load(Pet& pet, const std::string& path) {
    std::ifstream in(path);
    if (!in) return { LoadStatus::NoFile, "", 0 };

    // Reads the whole file into key -> value pairs.
    Values values;
    bool syntaxOk = true;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) { syntaxOk = false; break; }
        values[line.substr(0, eq)] = line.substr(eq + 1);
    }
    in.close();

    // Something is wrong with the file: keep it as "<path>.bad" and report why.
    auto fail = [&](const std::string& why) {
        std::error_code ec;
        fs::rename(path, path + ".bad", ec);
        return LoadResult{ LoadStatus::Corrupt, why, 0 };
        };

    if (!syntaxOk) return fail("line without '='");

    double version = 0;
    if (!readNumber(values, "version", true, version)) return fail("missing version");
    if (static_cast<int>(version) != kSaveVersion) return fail("unknown save version");
    if (values.find("name") == values.end()) return fail("missing name");

    // Build the pet on the side; only copy it into `pet` if everything checks out.
    Pet loaded(cleanName(values["name"]));

    double age = 0;
    if (!readNumber(values, "age", true, age)) return fail("bad age");
    loaded.ageSeconds_ = age < 0 ? 0 : age;

    struct StatField { const char* key; Stat* stat; };
    const StatField stats[] = {
        {"hunger",      &loaded.hunger_},
        {"happiness",   &loaded.happiness_},
        {"energy",      &loaded.energy_},
        {"cleanliness", &loaded.cleanliness_},
        {"health",      &loaded.health_},
    };
    for (const StatField& f : stats) {
        double x = 0;
        if (!readNumber(values, f.key, true, x)) return fail(std::string("bad ") + f.key);
        f.stat->set(static_cast<float>(x));  // set() clamps to 0-100
    }

    // Optional fields: fall back to the defaults if they're missing.
    double x = 0;
    if (!readNumber(values, "sleeping", false, x)) return fail("bad sleeping");
    if (values.count("sleeping")) loaded.sleeping_ = (x != 0);

    if (!readNumber(values, "sick", false, x)) return fail("bad sick");
    if (values.count("sick")) loaded.sick_ = (x != 0);

    if (!readNumber(values, "wants_play", false, x)) return fail("bad wants_play");
    if (values.count("wants_play")) loaded.wantsPlay_ = (x != 0);

    if (!readNumber(values, "play_request_timer", false, x)) return fail("bad play_request_timer");
    if (values.count("play_request_timer")) {
        loaded.playRequestTimer_ = std::clamp(static_cast<float>(x), 0.0f, 3600.0f);
    }

    if (!readNumber(values, "next_event_in", false, x)) return fail("bad next_event_in");
    if (values.count("next_event_in")) {
        loaded.nextEventIn_ = std::clamp(static_cast<float>(x), 1.0f, 3600.0f);
    }

    const char* cooldownKeys[kActionCount] = {
        "cooldown_feed", "cooldown_play", "cooldown_sleep", "cooldown_clean", "cooldown_medicine" };
    for (int i = 0; i < kActionCount; ++i) {
        if (!readNumber(values, cooldownKeys[i], false, x)) return fail(std::string("bad ") + cooldownKeys[i]);
        if (values.count(cooldownKeys[i])) {
            loaded.cooldown_[i] = std::clamp(static_cast<float>(x), 0.0f, 3600.0f);
        }
    }

    double savedAt = 0;
    if (!readNumber(values, "saved_at", false, savedAt)) return fail("bad saved_at");

    pet = loaded;
    return { LoadStatus::Ok, "", static_cast<long long>(savedAt) };
}

void SaveGame::erase(const std::string& path) {
    std::error_code ec;
    fs::remove(path, ec);
}