⚙️ Features added:
	- Hunger, happiness, energy and cleanliness fall at their own per-second rates.
	- Health drops for every one of hunger, energy and cleanliness that is at 0, and slowly recovers while all three are above 20.
	- While sleeping, the other stats decay at half speed and energy rises.
	- Config::kTimeScale is a debug multiplier. At 60 a whole life takes seconds, which makes decay and death easy to test.
	- All rates live in config.hpp.
	- While asleep, every action except Sleep is refused ("Pip is asleep. Wake up first.").
	- The pet wakes by itself when energy is full.
	- A new Cleanliness stat was added so Clean has something to do. Eating and playing make a mess.
	- Every refusal returns a readable reason, which goes into the message log.
	- Five stat bars with percentages.
	- Status line hat picks the most urgent problem and is color-coded: red for trouble, green for happy, blue for sleeping.
	- Action menu: Up/Down plus Enter, or hotkeys F, P, S, C and M. The selected row is highlighted, the Sleep entry becomes "Wake up" while sleeping, and each action shows its remaining cooldown in red.
	- Message log: last 5 messages, newest at the bottom and brightest.
	- Low-stat warnings: ("Pip is hungry!") appear once when a stat falls below 20 and re-arm once it recovers above 35.
	- Placeholder art has awake, sleeping (z Z, closed eyes) and dead (x.x) variants.
🧱 Updates to your system design:
	 The terminal went through many changes:
		1. A string buffer with a full screen clear every frame.
		2. Direct `printf` calls (a simplification).
		3. Current: an 80x30 in-memory grid of `{char, color}` cells, diffed row by row against what is already on screen. The screen is cleared once at startup and never again.
🧹 Refactoring improvements:
	- **Before:** a fixed 50 ms timestep with a lag accumulator (while (lag >= kTick)), a 250 ms frame clamp, and 16 ms sleeps.
	- **After:** measure `dt` since the last frame and call update(dt) once.
	- **Before**: Game::handleInput() had a switch that poked stats directly (pet_.hunger().add(20)) and set messages. It was marked "temporary" and put game rules in the UI.
	- **After**: Game::useAction() is two lines: call pet_.perform(action) and log the returned message. All rules, refusals and cooldowns live in Pet::perform().
💭 What you changed and why:
	Added Cleanliness
		 **Why**: Because i thought it would be good for another action called clean.
	Moved all rules into Pet and all numbers into Config
		**Why**: The pet can now be tested and tuned without touching UI code.
	