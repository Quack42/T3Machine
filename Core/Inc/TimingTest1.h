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

public:
	TimingTest1(
				OutputPin<Platform> & ldOrange,
				OutputPin<Platform> & ldBlue,
				Timer<Platform> & timer) :
			//References
			ldOrange(ldOrange),
			ldBlue(ldBlue),
			timer(timer)
			//Components
			//Variables
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
			//button is released
			ldBlue.low();

			//start timer test
			timer.setTime(TimeValue(0,1000,0)); 	// 1 second
			timer.start();
		}
	}

private:

	void timerISR() {
		ldOrange.low();
		// ldBlue.low();
	}

};
