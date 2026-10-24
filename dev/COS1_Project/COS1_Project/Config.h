#pragma once


// "per second" values are applied every frame based on the elapsed time.
namespace Config {

	// Debug helper: 1 = normal speed, 10 = everything happens 10x faster.
	constexpr float kTimeScale = 1.0f;

	// ---- Stat decay (points per second) ----
	constexpr float kHungerDecay = 0.30f;
	constexpr float kHappinessDecay = 0.20f;
	constexpr float kEnergyDecay = 0.15f;
	constexpr float kCleanlinessDecay = 0.12f;

	constexpr float kSleepDecayFactor = 0.5f;  // other stats drop half as fast while asleep
	constexpr float kSleepEnergyGain = 2.0f;  // energy regained per second while asleep

	constexpr float kHealthLoss = 0.5f;   // per second, for each stat that has hit 0
	constexpr float kHealthRegen = 0.1f;   // per second, when the pet is doing okay
	constexpr float kRegenMinStat = 20.0f;  // hunger/energy/cleanliness must be above this

	// ---- Mood (what state the pet is in) ----
	constexpr float kLowStat = 25.0f;  // below this a stat becomes a problem (hungry, tired, ...)
	constexpr float kHappyAbove = 60.0f;  // all four stats above this = happy

	// ---- Care actions ----
	// Feed
	constexpr float kFeedHunger = 25.0f;
	constexpr float kFeedHappiness = 3.0f;
	constexpr float kFeedMess = 3.0f;
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
	constexpr float kMedicineHappiness = 5.0f;   // nobody likes medicine
	constexpr float kHealthyAbove = 90.0f;  // healthy pets don't need medicine
	constexpr float kMedicineCooldown = 20.0f;

	// How long the "eating / playing / ..." animation lasts (real seconds)
	constexpr float kActivityDuration = 1.5f;

	// ---- Random events ----
	// Every so often (random time between these two) something happens.
	constexpr float kEventMinDelay = 45.0f;   // seconds
	constexpr float kEventMaxDelay = 120.0f;

	constexpr float kSickHealthHit = 10.0f;  // health lost the moment it gets sick
	constexpr float kSickHealthLoss = 0.25f;  // per second while sick (only medicine stops it)
	constexpr float kSickHappinessLoss = 0.10f;  // per second while sick

	constexpr float kPlayRequestTime = 30.0f;  // seconds you have to answer "wants to play"
	constexpr float kPlayRequestBonus = 10.0f;  // extra happiness if you do
	constexpr float kPlayIgnoredPenalty = 10.0f;  // happiness lost if you don't

	constexpr float kMessAmount = 25.0f;  // cleanliness lost when it makes a mess

	// ---- Game over and saving ----
	constexpr double kGameOverDelay = 3.0;   // seconds the dead pet is shown before the game over screen
	constexpr double kAutosaveInterval = 30.0;  // seconds between autosaves
	constexpr const char* kSaveFile = "pet.sav";  // saved in the folder you run the game from

}  