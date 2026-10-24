⚙️ Final features added:
	Mood and state machine:
		States:
			Dead
			Sleeping
			Eating
			Playing
			Cleaning
			Medication
			Sick
			Hungry
			Tired
			Dirty
			Sad
			Happy
			Content
	Animations and ASCII Art
	Death and Game Over
	Save System and Load on start up.

🧹 Refactoring improvements:
	Events replaced "compare before and after" detection in Game class
	Status text now comes from Pet::state()
	Hard-coded art replaced by the Animator
	Game class split into smaller functions
	Game class got a constructor; start-up logic moved out of run()
	Pet::update() now separates real time from game time
	config.hpp reorganized
	Pet::perform() cleanup
	friend class SaveGame instead of a wall of setters

🐛 Bug fixes:
	A sick pet with high health couldn't be cured.
		fix: Refusal rule changed to !sick_ && health > 90. Sickness is always curable. The message also changes ("is cured!" vs "takes the medicine. Yuck.").
	Corrupt save crashes the game or loads nonsense
		fix: Strict parsing: missing keys, non-numeric text, nan, inf, trailing junk like 50x, a line with no =, and unknown versions are all rejected. The game starts a new pet and keeps the bad file as .bad.

✨ Usability updates:
	Reading the pet at a glance with animations and new ASCII art
	Clearer messages
	Time pressure is visible for when the pet asks to play.
	Saving is automatic and visible
