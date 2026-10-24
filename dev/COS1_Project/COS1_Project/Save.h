#pragma once
#include <string>

#include "Pet.h"

enum class LoadStatus { Ok, NoFile, Corrupt };

struct LoadResult {
    LoadStatus status;
    std::string error;       // why it failed (only for Corrupt)
    long long savedAt = 0;   // unix time of the save; used later for offline time simulation
};

// Saves and loads a Pet as a small text file of key=value lines.
class SaveGame {
public:
    // Returns false if the file couldn't be written.
    // Writes to "<path>.tmp" first and then renames, so a crash while saving
    // can never leave you with a half-written save file.
    static bool save(const Pet& pet, const std::string& path);

    // Fills `pet` only if the whole file is valid; otherwise `pet` is untouched.
    // A corrupt file is renamed to "<path>.bad" instead of being deleted.
    static LoadResult load(Pet& pet, const std::string& path);

    static void erase(const std::string& path);
};