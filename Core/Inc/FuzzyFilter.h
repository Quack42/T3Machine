#pragma once

#include <cmath>

#include "StopWatch.h"

//TODO: Continue here: The sensor input isn't nice and clear, as expectable, so I'd like to add a bit of a fuzzy filter concept to it. Unfortunately, there is a process-flow 'hindrance' in my architecture design. I'd like to switch to a process flow that follows the input-data (e.g. interrupts call it's subscribers, like fuzzy logic, which in turn calls t3machine if a value changes, etc.)

template <typename Input, typename Platform>
class FuzzyFilter {
private:
	//References
	TimingManager<Platform> & timingManager;
	Input & input;

	//Components
	StopWatch<Platform> stopwatch;
	TimeCountDown switchTime;

	//Variables
	float lowBound = 0.3f;
	float highBound = 0.7;
	float fullSwitchTime = 1.f; 	//time (in ms) it takes for fuzzyBoolean to go from 0.0f to 1.0f;
	float fuzzyBoolean = 0.5f;
	bool fuzzyBooleanState = false;
	bool lastValue = false;

public:
	FuzzyFilter(TimingManager<Platform> & timingManager, Input & input, float lowBound = 0.3f, float highBound = 0.7f, float fullSwitchTime = 1.f) :
			//References
			timingManager(timingManager),
			input(input),
			//Components
			stopwatch(timingManager),
			//Variables
			lowBound(lowBound),
			highBound(highBound)
	{

	}

	void inputUpdate() {
		bool newValue = input.getValue();

		float timePassed = 0.f; //TODO: get time since last update 	
		float ratio = timePassed/fullSwitchTime;

		//Adjust fuzzyBoolean accordingly (based on 'lastValue' and time since last update)
		if (lastValue) {
			//input is high; so add ratio
			fuzzyBoolean += ratio;
			
		} else {
			//input is high; so add ratio
			fuzzyBoolean -= ratio;
		}

		//clamp to range
		if (fuzzyBoolean > 1.f) {
			//clamp to [0.0f, 1.0f] range
			fuzzyBoolean = 1.f;
		}
		if (fuzzyBoolean < 0.f) {
			//clamp to [0.0f, 1.0f] range
			fuzzyBoolean = 0.f;
		}

		//check state of fuzzyBooleanState
		if (fuzzyBoolean > highBound) {
			fuzzyBooleanState = true;
			//TODO: advertise
		}
		if (fuzzyBoolean < lowBound) {
			fuzzyBooleanState = false;
			//TODO: advertise
		}

		//set timer for next switch; if switch is coming (based on 'newValue')
		if (fuzzyBooleanState != newValue) {
			//heading to fuzzyBooleanState switch
			//count down till expected switch
			//update switch time
			if (newValue) {
				//fuzzyBoolean value going towards 'highBound'
				switchTime.setTimeCountDown(highBound-fuzzyBoolean); 	//by above computation, 'fuzzyBoolean' should always be smaller than 'highBound' at this point.
			} else {
				//fuzzyBoolean value going towards 'lowBound'
				switchTime.setTimeCountDown(fuzzyBoolean-lowBound); 	//by above computation, 'fuzzyBoolean' should always be bigger than 'lowBound' at this point.
			}
			timingManager.scheduleWakeup(switchTime.getRemainingTime());
		}
		lastValue = newValue;

	}

	void init() {
		lastValue = input.getValue();
		stopwatch.start();
		initSwitchTime();
	}

	bool getValue() {
		return fuzzyBooleanState;
	}

	Input & getRawInput() {
		return input;
	}

	// void tick(float timePassed) {
	// 	float ratio = timePassed/fullSwitchTime;
	// 	if (input.getValue()) {
	// 		//input is high; so add ratio
	// 		fuzzyBoolean += ratio;
	// 		if (fuzzyBoolean > 1.f) {
	// 			//clamp to [0.0f, 1.0f] range
	// 			fuzzyBoolean = 1.f;
	// 		}
	// 		if (fuzzyBoolean > highBound) {
	// 			fuzzyBooleanState = true;
	// 		}
	// 	} else {
	// 		//input is high; so add ratio
	// 		fuzzyBoolean -= ratio;
	// 		if (fuzzyBoolean < 0.f) {
	// 			//clamp to [0.0f, 1.0f] range
	// 			fuzzyBoolean = 0.f;
	// 		}
	// 		if (fuzzyBoolean < lowBound) {
	// 			fuzzyBooleanState = false;
	// 		}
	// 	}
	// 	updateSwitchTime();
	// }

private:
	void initSwitchTime() {
		float minimumTimeToSwitch =
			std::min(
				std::abs(fuzzyBoolean-lowBound),
				std::abs(fuzzyBoolean-highBound)
			);
		switchTime.setTimeCountDown(minimumTimeToSwitch);
		timingManager.scheduleWakeup(switchTime.getRemainingTime());
	}
};

