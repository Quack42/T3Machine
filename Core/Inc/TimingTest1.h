#pragma once

#include "ProcessManager.h"
#include "TimingManager.h"
#include "MyGPIO.h"
#include "StopWatch.h"
#include "Timer.h"

template<typename Platform>
class TimingTest1 {
private:
	//References
	OutputPin<Platform> & ldOrange;
	OutputPin<Platform> & ldBlue;
	Timer<Platform> & timer;
	//Components
	//Variables
	int counter;

public:
	TimingTest1(
				OutputPin<Platform> & ldOrange,
				OutputPin<Platform> & ldBlue,
				Timer<Platform> & timer) :
			//References
			ldOrange(ldOrange),
			ldBlue(ldBlue),
			timer(timer),
			//Components
			//Variables
			counter(0)
	{
		
	}

	void init() {
		timer.setISRCallbackFunction(std::bind(&TimingTest1::timerISR, this));
	}

	void input(bool inputValue) {
		if (inputValue) {
			//button is pressed
			ldOrange.high();
			ldBlue.high();
			timer.stop();
		} else {
			counter = 0;
			//button is released
			ldBlue.low();

			//start timer test
			timer.setTime(TimeValue(0,1000,0)); 	// 1 second
			timer.start();
		}
	}

private:

	void timerISR() {
		ldOrange.toggle();
		counter++;

		if (counter & 1) {
			timer.setTime(TimeValue(0.05f * (201-(counter%200)))); 	// 1 second
		} else {
			timer.setTime(TimeValue(0.05f * (1+counter%200))); 	// 1 second
		}
		if (counter < 2000){
			timer.start();
		} else {
			ldOrange.low();
		}

		// ldOrange.low();
		// ldBlue.low();
	}

};
