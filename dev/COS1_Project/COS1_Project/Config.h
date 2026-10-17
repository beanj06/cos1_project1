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


	// Feed
	constexpr float kFeedHunger = 25.0f;
	constexpr float kFeedHappiness = 3.0f;
	constexpr float kFeedMess = 3.0f;   // eating makes a little mess
	constexpr float kFeedCooldown = 2.0f;   // seconds
	constexpr float kFullAbove = 95.0f;  // won't eat if hunger is above this

	// Play
	constexpr float kPlayHappiness = 20.0f;
	constexpr float kPlayEnergy = 15.0f;  // energy spent
	constexpr float kPlayHunger = 5.0f;   // hunger spent
	constexpr float kPlayMess = 5.0f;
	constexpr float kPlayMinEnergy = 15.0f;  // too tired to play below this
	constexpr float kPlayCooldown = 3.0f;

	// Sleep
	constexpr float kRestedAbove = 90.0f;  // not tired enough to sleep above this
	constexpr float kSleepCooldown = 5.0f;   // after waking up

	// Clean
	constexpr float kCleanAmount = 50.0f;
	constexpr float kCleanAbove = 90.0f;  // already clean above this
	constexpr float kCleanCooldown = 5.0f;

	// Medicine
	constexpr float kMedicineHealth = 30.0f;
	constexpr float kMedicineHappiness = 5.0f;  // nobody likes medicine
	constexpr float kHealthyAbove = 90.0f;  // doesn't need medicine above this
	constexpr float kMedicineCooldown = 20.0f;

} 