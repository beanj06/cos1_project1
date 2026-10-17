#pragma once

// All the numbers you might want to tune live here.
// "per second" values are applied every frame based on the elapsed time.
namespace Config {

	// Debug helper: 1 = normal speed, 10 = everything happens 10x faster.
	constexpr float kTimeScale = 1.0f;

	constexpr float kHungerDecay = 0.30f;
	constexpr float kHappinessDecay = 0.20f;
	constexpr float kEnergyDecay = 0.15f;
	constexpr float kCleanlinessDecay = 0.12f;

	constexpr float kSleepDecayFactor = 0.5f;  // other stats drop half as fast while asleep
	constexpr float kSleepEnergyGain = 2.0f;  // energy regained per second while asleep

	constexpr float kHealthLoss = 0.5f;  // per second, for each stat that has hit 0
	constexpr float kHealthRegen = 0.1f;  // per second, when the pet is doing okay
	constexpr float kRegenMinStat = 20.0f;  // hunger/energy/cleanliness must be above this

}