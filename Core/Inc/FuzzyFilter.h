#pragma once

#include "ProcessManager.h"
#include "TimingManager.h"
#include "StopWatch.h"
#include "TimedTask.h"
#include "ProcessRequest.h"

#include <cmath>
#include <cstdint>
#include <functional>

//TODO: Continue here: The sensor input isn't nice and clear, as expectable, so I'd like to add a bit of a fuzzy filter concept to it. Unfortunately, there is a process-flow 'hindrance' in my architecture design. I'd like to switch to a process flow that follows the input-data (e.g. interrupts call it's subscribers, like fuzzy logic, which in turn calls t3machine if a value changes, etc.)

template <typename Platform>
class FuzzyFilter {
private:
	//References
	ProcessManager<Platform> & processManager;
	TimingManager<Platform> & timingManager;

	//Components
	StopWatch<Platform> stopWatch;
	TimedTask switchFuzzyBooleanStateTask;
	ProcessRequest advertiseTask;
	std::function<void(bool)> subscriberFunction;

	//Variables
	float fullSwitchTime = 1.0f; 	//time (in ms) it takes for fuzzyBoolean to go from 0.0f to 1.0f;
	float lowBound = 0.3f;
	float highBound = 0.7;
	float fuzzyBoolean = 0.5f;
	bool fuzzyBooleanState = false;
	bool lastValue = false;

public:
	FuzzyFilter(ProcessManager<Platform> & processManager, TimingManager<Platform> & timingManager, float fullSwitchTime = 1.0f, float lowBound = 0.3f, float highBound = 0.7f) :
			//References
			processManager(processManager),
			timingManager(timingManager),
			//Components
			stopWatch(timingManager),
			switchFuzzyBooleanStateTask(std::bind(&FuzzyFilter::switchFuzzyBooleanState, this), TimeValue(0,0,0)),
			advertiseTask(std::bind(&FuzzyFilter::advertise, this)),
			//Variables
			fullSwitchTime(fullSwitchTime),
			lowBound(lowBound),
			highBound(highBound)
	{

	}

	void init(bool initialValue) {
		lastValue = initialValue;
		fuzzyBooleanState = initialValue;
		stopWatch.start();
		updateTimer(initialValue);
		processManager.requestProcess(advertiseTask);
	}

	void input(bool inputValue) {
		updateFuzzyBoolean();
		//update lastValue with new value
		lastValue = inputValue; 		//NOTE: Not sure about the order of execution; might want to move this to the start of the function.
		updateTimer(inputValue);
	}

	void switchFuzzyBooleanState() {
		if (lastValue != fuzzyBooleanState) {
			fuzzyBooleanState = lastValue;
			processManager.requestProcess(advertiseTask);
		}
		// updateFuzzyBoolean(); 	//NOTE: not necessary..
	}

	void advertise() {
		if (subscriberFunction) {
			subscriberFunction(lastValue);
		}
	}

	void setSubscriberFunction(std::function<void(bool)> subscriberFunction) {
		this->subscriberFunction = subscriberFunction;
	}

	bool getValue() {
		return fuzzyBooleanState;
	}

private:
	void updateFuzzyBoolean() {
		TimeValue stopWatchTime = stopWatch.getTime();
		stopWatch.start();
		//Convert TimeValue to float (in ms)
		float timePassed = 0.0f;
		timePassed += static_cast<float>(stopWatchTime.days) * MS_IN_A_DAY; 	//days
		timePassed += stopWatchTime.ms; 										//ms
		timePassed += static_cast<float>(stopWatchTime.us) / US_IN_A_MS; 		//us


		//Get the ratio to the switch-time (e.g. the value it would have changed)
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
		if (fuzzyBoolean > 1.0f) {
			//clamp to [0.0f, 1.0f] range
			fuzzyBoolean = 1.0f;
		}
		if (fuzzyBoolean < 0.0f) {
			//clamp to [0.0f, 1.0f] range
			fuzzyBoolean = 0.0f;
		}

		//check state of fuzzyBooleanState
		if ((fuzzyBoolean > highBound) && !fuzzyBooleanState) {
			fuzzyBooleanState = true;
			processManager.requestProcess(advertiseTask);
		}
		if ((fuzzyBoolean < lowBound) && fuzzyBooleanState) {
			fuzzyBooleanState = false;
			processManager.requestProcess(advertiseTask);
		}
	}

	void updateTimer(bool inputValue) {
		//set timer for next switch; if switch is coming (based on 'inputValue')
		if (fuzzyBooleanState != inputValue) {
			//heading to fuzzyBooleanState switch
			//count down till expected switch
			//update switch time
			float ratioToTraverseUntilSwitch = 0.0f;
			if (inputValue) {
				//fuzzyBoolean value going towards 'highBound'
				ratioToTraverseUntilSwitch = highBound-fuzzyBoolean; 	//by above computation, 'fuzzyBoolean' should always be smaller than 'highBound' at this point.
			} else {
				//fuzzyBoolean value going towards 'lowBound'
				ratioToTraverseUntilSwitch = fuzzyBoolean-lowBound; 	//by above computation, 'fuzzyBoolean' should always be bigger than 'lowBound' at this point.
			}
			float msUntilExpectedSwitch = ratioToTraverseUntilSwitch * fullSwitchTime;
			//float to TimeValue
			TimeValue timeUntilExpectedSwitch(
				static_cast<uint32_t>(msUntilExpectedSwitch/MS_IN_A_DAY), 				//the cast cuts off the decimal numbers (i.e. anything less than a day)
				static_cast<uint32_t>(msUntilExpectedSwitch) % MS_IN_A_DAY, 			//the cast cuts off the decimal numbers (i.e. anything less than a ms). Modulo is to ignore everything above the ms-in-a-day domain.
				static_cast<uint32_t>(msUntilExpectedSwitch*US_IN_A_MS + 0.5f) % US_IN_A_MS 	//convert ms to us, cast is required for modulo (+0.5f is for proper rounding). Modulo is to ignore everything above the us domain.
			);

			//Schedule wake up:
			switchFuzzyBooleanStateTask.setTimeUntilTaskStart(timeUntilExpectedSwitch);
			timingManager.addTask(&switchFuzzyBooleanStateTask);
		}
	}
};
