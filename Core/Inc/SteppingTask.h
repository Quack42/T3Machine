#pragma once

#include "Timing.h"

//TODO: adjust SteppingTask to new TimingManager

template<typename Driver, typename Platform>
class SteppingTask {
private:
	constexpr static float kDefaultStepHighTime = 0.5f;
	constexpr static float kDefaultStepLowTime = 0.5f;
	//variables
	float stepHighTime;
	float stepLowTime;
	TimeCountDown stepTime;
	int steps;
	enum {
		e_idle,
		e_toInitiate,
		e_toStepHigh,
		e_toStepLow
	} state = e_idle;
	bool stopping;
	//references
	TimingManager<Platform> & timingManager;
	Driver & driver;
	int & positionIndex;

public:
	SteppingTask(TimingManager<Platform> & timingManager, Driver & driver, int & positionIndex, float stepHighTime = kDefaultStepHighTime, float stepLowTime = kDefaultStepLowTime) :
			//constants
			stepHighTime(stepHighTime),
			stepLowTime(stepLowTime),
			//variables
			stepTime(stepLowTime),
			steps(0),
			state(e_idle),
			stopping(false),
			//references
			timingManager(timingManager),
			driver(driver),
			positionIndex(positionIndex)
	{

	}

	void setStepHighTime(const float & stepHighTime) {
		this->stepHighTime = stepHighTime;
	}

	void setStepLowTime(const float & stepLowTime) {
		this->stepLowTime = stepLowTime;
	}

	bool isActive() {
		return (state != e_idle);
	}

	void startSteppingTask(int steps) {
		this->steps = steps;
		stopping = false;

		if (stepTime.isReady()) {
			initiateStepping();
		} else {
			state = e_toInitiate;
			timingManager.scheduleWakeup(stepTime.getRemainingTime());
		}
	}

	void stop() {
		stopping = true;
	}

	void tick(float timePassed) {
		stepTime.countDown(timePassed);
		if (stepTime.isReady()) {
			switch(state) {
				case e_idle:
					//do nothing
					break;
				case e_toInitiate:
					initiateStepping();
					break;
				case e_toStepHigh:
					stepHigh();
					break;
				case e_toStepLow:
					stepLow();
					break;
				default:
					//do nothing
					break;
			}
		}
	}

private:
	void initiateStepping() {
		//set driver to step to appropriate direction
		bool directionHigh = (steps > 0);
		driver.setDirectionPin(directionHigh);
		//step
		stepHigh();
	}

	//task functions
	// void step() {
	// 	driver.step();
	// 	timingManager.scheduleWakeup();
	// }

	void stepHigh() {
		driver.setStepPin(true);
		stepTime.setTimeCountDown(stepHighTime);
		timingManager.scheduleWakeup(stepTime.getRemainingTime());
		state = e_toStepLow;
	}

	void stepLow() {
		driver.setStepPin(false);
		positionIndex++; 	//update position index
		stepTime.setTimeCountDown(stepLowTime);
		timingManager.scheduleWakeup(stepTime.getRemainingTime());
		//reduce number of steps to take
		if (steps > 0) {
			steps--;
		} else if (steps < 0) {
			steps++;
		}
		//check if steps are done
		if (steps == 0 || stopping) {
			//steps are done
			state = e_idle;
		} else {
			//need another step
			state = e_toStepHigh;
		}
	}
};
