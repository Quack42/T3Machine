#pragma once

//TODO: Continue here: The sensor input isn't nice and clear, as expectable, so I'd like to add a bit of a fuzzy filter concept to it. Unfortunately, there is a process-flow 'hindrance' in my architecture design. I'd like to switch to a process flow that follows the input-data (e.g. interrupts call it's subscribers, like fuzzy logic, which in turn calls t3machine if a value changes, etc.)

template <typename Input, typename Platform>
class FuzzyFilter_Interrupt {
private:
	//References
	TimingManager<Platform> & timingManager
	Input & input;
	TimeCountDown switchTime;

	//Variables
	float lowBound = 0.3f;
	float highBound = 0.7;
	float fullSwitchTime = 1.f; 	//time (in ms) it takes for fuzzyBoolean to go from 0.0f to 1.0f;
	float fuzzyBoolean = 0.5f;
	bool fuzzyBooleanState = false;

public:
	FuzzyFilter(TimingManager<Platform> & timingManager, Input & input, float lowBound = 0.3f, float highBound = 0.7f, float fullSwitchTime = 1.f) :
			timingManager(timingManager),
			input(input),
			lowBound(lowBound),
			highBound(highBound)
	{

	}

	void init() {
		updateSwitchTime();
	}

	bool getValue() {
		return fuzzyBooleanState;
	}

	Input & getRawInput() {
		return input;
	}

	void tick(float timePassed) {
		float ratio = timePassed/fullSwitchTime;
		if (input.getValue()) {
			//input is high; so add ratio
			fuzzyBoolean += ratio;
			if (fuzzyBoolean > 1.f) {
				//clamp to [0.0f, 1.0f] range
				fuzzyBoolean = 1.f;
			}
			if (fuzzyBoolean > highBound) {
				fuzzyBooleanState = true;
			}
			//input is high; so add ratio
			fuzzyBoolean -= ratio;
			if (fuzzyBoolean < 0.f) {
				//clamp to [0.0f, 1.0f] range
				fuzzyBoolean = 0.f;
			}
			if (fuzzyBoolean < lowBound) {
				fuzzyBooleanState = false;
			}
		}
		updateSwitchTime();
	}

private:
	void updateSwitchTime() {
		float minimumTimeToSwitch =
			std::min(
				std::abs(fuzzyBoolean-lowBound),
				std::abs(fuzzyBoolean-highBound)
			);
		switchTime.setTimeCountDown(minimumTimeToSwitch);
		timingManager.scheduleWakeup(switchTime.getRemainingTime());
	}
};

#pragma once

template <typename Input, typename Platform>
class FuzzyFilter_Polling {
private:
	//References
	TimingManager<Platform> & timingManager
	Input & input;
	TimeCountDown switchTime;

	//Variables
	float lowBound = 0.3f;
	float highBound = 0.7;
	float fullSwitchTime = 1.f; 	//time (in ms) it takes for fuzzyBoolean to go from 0.0f to 1.0f;
	float fuzzyBoolean = 0.5f;
	bool fuzzyBooleanState = false;

public:
	FuzzyFilter(TimingManager<Platform> & timingManager, Input & input, float lowBound = 0.3f, float highBound = 0.7f, float fullSwitchTime = 1.f) :
			timingManager(timingManager),
			input(input),
			lowBound(lowBound),
			highBound(highBound)
	{

	}

	void init() {
		updateSwitchTime();
	}

	bool getValue() {
		return fuzzyBooleanState;
	}

	Input & getRawInput() {
		return input;
	}

	void tick(float timePassed) {
		float ratio = timePassed/fullSwitchTime;
		if (input.getValue()) {
			//input is high; so add ratio
			fuzzyBoolean += ratio;
			if (fuzzyBoolean > 1.f) {
				//clamp to [0.0f, 1.0f] range
				fuzzyBoolean = 1.f;
			}
			if (fuzzyBoolean > highBound) {
				fuzzyBooleanState = true;
			}
			//input is high; so add ratio
			fuzzyBoolean -= ratio;
			if (fuzzyBoolean < 0.f) {
				//clamp to [0.0f, 1.0f] range
				fuzzyBoolean = 0.f;
			}
			if (fuzzyBoolean < lowBound) {
				fuzzyBooleanState = false;
			}
		}
		updateSwitchTime();
	}

private:
	void updateSwitchTime() {
		float minimumTimeToSwitch =
			std::min(
				std::abs(fuzzyBoolean-lowBound),
				std::abs(fuzzyBoolean-highBound)
			);
		switchTime.setTimeCountDown(minimumTimeToSwitch);
		timingManager.scheduleWakeup(switchTime.getRemainingTime());
	}
};