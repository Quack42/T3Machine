#pragma once

#include "ProcessManager.h"
#include "TimingManager.h"
#include "MyGPIO.h"

template<typename Platform>
class TimingTest0 {
private:
	ProcessManager<Platform> & processManager;
	TimingManager<Platform> & timingManager;
	PinData<Platform> & ld;
	TimedTask lightShowTimedCallbackTask;
	enum LightShowState_e {
		e_0,
		e_1,
		e_2,
		e_3,
		e_4,
		e_5
	} lightShowState;

public:
	TimingTest0(ProcessManager<Platform> & processManager, TimingManager<Platform> & timingManager, PinData<Platform> & ld) :
			processManager(processManager),
			timingManager(timingManager),
			ld(ld),
			lightShowTimedCallbackTask(std::bind(&TimingTest0::lightShowTimedCallback, this), TimeValue(0,1000,0))
	{

	}

	void input(bool inputValue) {
		if (inputValue) {
			//button is pressed
			ld.high();
		} else {
			//button is released
			ld.low();
			//Start light show.
			lightShowTimedCallbackTask.setTimeUntilTaskStart(TimeValue(0,1000,0));
			timingManager.addTask(&lightShowTimedCallbackTask);
		}
	}

private:
	void lightShowTimedCallback() {
		//state action
		switch(lightShowState) {
			default:
			case e_0:
			case e_2:
			case e_4:
				ld.high();
				lightShowTimedCallbackTask.setTimeUntilTaskStart(TimeValue(0,1000,0));
				timingManager.addTask(&lightShowTimedCallbackTask);
				break;
			case e_1:
			case e_3:
				ld.low();
				lightShowTimedCallbackTask.setTimeUntilTaskStart(TimeValue(0,1000,0));
				timingManager.addTask(&lightShowTimedCallbackTask);
				break;
			case e_5:
				ld.low();
				break;
		}

		//state progression
		switch(lightShowState) {
			default:
			case e_0:
				lightShowState = e_1;
				break;
			case e_1:
				lightShowState = e_2;
				break;
			case e_2:
				lightShowState = e_3;
				break;
			case e_3:
				lightShowState = e_4;
				break;
			case e_4:
				lightShowState = e_5;
				break;
			case e_5:
				lightShowState = e_0;
				break;
		}
	}
};
