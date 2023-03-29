#pragma once

#include "ProcessManager.h"
#include "TimingManager.h"
#include "MyGPIO.h"
#include "StopWatch.h"

template<typename Platform>
class TimingTest0 {
private:
	//References
	ProcessManager<Platform> & processManager;
	TimingManager<Platform> & timingManager;
	OutputPin<Platform> & ldOrange;
	OutputPin<Platform> & ldBlue;
	//Components
	TimedTask lightShowTimedCallbackTask;
	// TimedTask cancelLightShowTaskTask;
	StopWatch<Platform> stopwatch;
	//Variables
	enum LightShowState_e {
		e_0,
		e_1,
		e_2,
		e_3,
		e_4,
		e_5,
		e_6,
		e_7
	} lightShowState;


public:
	TimingTest0(ProcessManager<Platform> & processManager, TimingManager<Platform> & timingManager, OutputPin<Platform> & ldOrange, OutputPin<Platform> & ldBlue) :
			//References
			processManager(processManager),
			timingManager(timingManager),
			ldOrange(ldOrange),
			ldBlue(ldBlue),
			//Components
			lightShowTimedCallbackTask(std::bind(&TimingTest0::lightShowTimedCallback, this), TimeValue(0,1000,0)),
			// cancelLightShowTaskTask(std::bind(&TimingTest0::cancelLightShowTaskCallback, this), TimeValue(0,11500,0)),
			stopwatch(timingManager),
			//Variables
			lightShowState(e_0)
	{

	}

	void input(bool inputValue) {
		if (inputValue) {
			//button is pressed
			ldOrange.high();
			ldBlue.high();
		} else {
			//button is released
			ldOrange.low();
			ldBlue.low();

			if (lightShowState == e_0) {
				//Start light show.
				lightShowTimedCallbackTask.setTimeUntilTaskStart(TimeValue(0,1000,0)); 	//TODO: uncomment
				timingManager.addTask(&lightShowTimedCallbackTask); 	//TODO: uncomment
				// cancelLightShowTask.setTimeUntilTaskStart(TimeValue(0,11500,0)); 	//TODO: uncomment
				// timingManager.addTask(&cancelLightShowTask); 	//TODO: uncomment
			}
		}
	}

private:
	// void cancelLightShowTaskCallback () {
		// timingManager.removeTask(&lightShowTimedCallbackTask);
	// }

	void lightShowTimedCallback() {
		//state action
		switch(lightShowState) {
			default:
			case e_2:
				ldBlue.high();
			case e_0:
				ldOrange.high();
				if (lightShowState == e_0) {
					stopwatch.start();
				}
				lightShowTimedCallbackTask.setTimeUntilTaskStart(TimeValue(0,1000,0));
				timingManager.addTask(&lightShowTimedCallbackTask);
				break;
			case e_6:
			case e_4:
				ldBlue.high();
				ldOrange.high();
				lightShowTimedCallbackTask.setTimeUntilTaskStart(TimeValue(0,1000,0));
				timingManager.addTask(&lightShowTimedCallbackTask);
				break;

			case e_1:
			case e_3:
				ldOrange.low();
				ldBlue.low();
				lightShowTimedCallbackTask.setTimeUntilTaskStart(TimeValue(0,1000,0));
				timingManager.addTask(&lightShowTimedCallbackTask);
				break;

			case e_5: {
				ldOrange.low();
				// ldBlue.low();
				TimeValue timeSpentFromState0ToState5 = stopwatch.getTime();
				lightShowTimedCallbackTask.setTimeUntilTaskStart(timeSpentFromState0ToState5);
				timingManager.addTask(&lightShowTimedCallbackTask);
				break;

			}
			case e_7:
				ldOrange.low();
				ldBlue.low();
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
				lightShowState = e_6;
				break;
			case e_6:
				lightShowState = e_7;
				break;
			case e_7:
				lightShowState = e_0;
				break;
		}
	}
};
