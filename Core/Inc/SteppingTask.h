#pragma once

#include "Timing.h"

template<typename Driver, typename Platform>
class SteppingTask {
private:
	constexpr static float kStepHighTime = 0.5f;
	constexpr static float kStepLowTime = 0.5f;
	//variables
	TimeCountDown stepTime;
	int steps;
	enum {
		e_idle,
		e_toInitiate,
		e_toStepHigh,
		e_toStepLow
	} state;
	bool stopping;
	//references
	Driver & driver;
	TimingManager<Platform> & timingManager;

public:
	SteppingTask(Driver & driver, TimingManager<Platform> & timingManager) :
			//variables
			stepTime(kStepLowTime),
			steps(0),
			state(e_idle),
			stopping(false),
			//references
			driver(driver),
			timingManager(timingManager)
	{

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
				default:
				case e_idle:
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
	void step() {
		driver.step();
		timingManager.scheduleWakeup();
	}

	void stepHigh() {
		driver.setStepPin(true);
		stepTime.setTimeCountDown(kStepHighTime);
		timingManager.scheduleWakeup(stepTime.getRemainingTime());
		state = e_toStepLow;
	}

	void stepLow() {
		driver.setStepPin(false);
		stepTime.setTimeCountDown(kStepLowTime);
		timingManager.scheduleWakeup(stepTime.getRemainingTime());
		//reduce number of steps to take
		if (steps > 0) {
			steps--;
		} else if (steps < 0) {
			steps++;
		}
		//check if steps are done
		if (steps == 0) {
			//steps are done
			state = e_idle;
		} else {
			//need another step
			state = e_toStepHigh;
		}
	}
};
