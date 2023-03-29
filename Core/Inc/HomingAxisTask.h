#pragma once

#include "ProcessManager.h"
#include "SubscriberLL.h"

#include <functional>


template<typename Driver, typename Platform>
class HomingAxisTask {
private:
	// References
	ProcessManager<Platform> & processManager;
	SteppingTask<Driver, Platform> & steppingTask;
	int & positionIndex;

	// Components
	Subscriber steppingTaskStopSubscription;
	Subscriber * homingStoppedSubscriptionList = nullptr;

	// Variables
	enum {
		e_idle,
		e_waitingForSteppingTaskToIdle_toStart,
		e_towardsSensorFast,
		e_fromSensorFast,
		e_towardsSensorSlow,
		e_done,
		//--- emergency stop
		e_waitingForSteppingTaskToIdle_toStop,
	} state_e = e_idle;
	bool lastSensorValue = false;


public:
	HomingAxisTask(	ProcessManager<Platform> & processManager,
					SteppingTask<Driver, Platform> & steppingTask,
					int & positionIndex) :
			processManager(processManager),
			steppingTask(steppingTask),
			positionIndex(positionIndex),
			steppingTaskStopSubscription(std::bind(&HomingAxisTask::steppingTaskStopped, this)),
			state_e(e_idle)
	{
	}

	void subscribeToStop(Subscriber * sub) {
		Subscriber::addSubscription(sub, &homingStoppedSubscriptionList);
	}

	void start() {
		if (steppingTask.isActive()) {
			stateSwitch_waitingForSteppingTaskToIdle_toStart();
		} else if (state_e == e_idle || state_e == e_done) {
			stateSwitch_towardsSensorFast();
		}
	}

	bool isActive() {
		return 		(state_e != e_idle)
				&& (state_e != e_done);
	}

	bool isDone() {
		return state_e == e_done;
	}

	void stop() {
		if (isActive()) {
			stateSwitch_waitingForSteppingTaskToIdle_toStop();
		}
	}

	void steppingTaskStopped() {
		//callback from stepping task to inform the stepping task has stopped and gone to idle. Ready for a new job!
		switch(state_e) {
			case e_idle:
				//do nothing
				break;
			case e_waitingForSteppingTaskToIdle_toStart:
				waitingForSteppingTaskToIdle_toStart();
				break;
			case e_towardsSensorFast:
				towardsSensorFast();
				break;
			case e_fromSensorFast:
				fromSensorFast();
				break;
			case e_towardsSensorSlow:
				toSensorSlow();
				break;
			case e_done:
				//do nothing
				break;
			case e_waitingForSteppingTaskToIdle_toStop:
				waitingForSteppingTaskToIdle_toStop();
				break;
			default:
				//do nothing
				break;
		}
	}

	void input_sensor(bool sensorValue) {
		lastSensorValue = sensorValue;
	}

private:

	void advertiseStop() {
		/// Advertise
		while (homingStoppedSubscriptionList != nullptr) {
			//first remove first from list
			Subscriber * current = homingStoppedSubscriptionList;
			homingStoppedSubscriptionList = homingStoppedSubscriptionList->getNext();
			//then add to process managers to-call list.
			processManager.requestProcess(current->getProcessRequest());
		}
	}


	/// -- State functions -- ///
	void stateSwitch_waitingForSteppingTaskToIdle_toStart() {
		steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		state_e = e_waitingForSteppingTaskToIdle_toStart;
		steppingTask.stop();
	}

	void waitingForSteppingTaskToIdle_toStart() {
		if (!steppingTask.isActive()) {
			//steppingTask stopped
			stateSwitch_towardsSensorFast();
		} else {
			steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		}
	}

	void stateSwitch_towardsSensorFast() {
		if (lastSensorValue) {
			//already there
			stateSwitch_fromSensorFast();
		} else {
			//not yet at the sensor; make a step towards it
			state_e = e_towardsSensorFast;
			steppingTask.subscribeToStop(&steppingTaskStopSubscription);
			steppingTask.setStepHighTime(0.5f); 	//TODO: get this magic number from a definitions list ("homing_fast_stepHighTime")
			steppingTask.setStepLowTime(0.5f); 	//TODO: get this magic number from a definitions list ("homing_fast_stepLowTime")
			steppingTask.startSteppingTask(1); 	//cw?
		}
	}

	void towardsSensorFast() {
		//wait for steppingtask to be done
		if (!steppingTask.isActive()) {
			//done with step
			//check sensor
			if (lastSensorValue) {
				//done with state; go to next state
				stateSwitch_fromSensorFast();
			} else {
				//done with step; initiate next step
				steppingTask.subscribeToStop(&steppingTaskStopSubscription);
				steppingTask.startSteppingTask(1); 	//cw?
			}
		} else {
			steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		}
	}

	void stateSwitch_fromSensorFast() {
		state_e = e_fromSensorFast;
		steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		steppingTask.setStepHighTime(0.5f); 	//TODO: get this magic number from a definitions list ("homing_fast_stepHighTime")
		steppingTask.setStepLowTime(0.5f); 	//TODO: get this magic number from a definitions list ("homing_fast_stepLowTime")
		steppingTask.startSteppingTask(-10); 	//ccw?
	}

	void fromSensorFast() {
		if (!steppingTask.isActive()) {
			//done stepping back
			if (!lastSensorValue) {
				//out of sensor, go to next state
				stateSwitch_toSensorSlow();
			} else {
				//but still in sensor, so keep going
				steppingTask.subscribeToStop(&steppingTaskStopSubscription);
				steppingTask.startSteppingTask(-10); 	//ccw?
			}
		} else {
			steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		}
	}

	void stateSwitch_toSensorSlow() {
		state_e = e_towardsSensorSlow;
		steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		steppingTask.setStepHighTime(0.1f); 	//TODO: get this magic number from a definitions list ("homing_slow_stepHighTime")
		steppingTask.setStepLowTime(49.9f); 	//TODO: get this magic number from a definitions list ("homing_slow_stepLowTime")
		steppingTask.startSteppingTask(1); 	//ccw?		
	}
	
	void toSensorSlow() {
		if (!steppingTask.isActive()) {
			if (lastSensorValue) {
				//done with state (found the sensor, slowly); go to done state
				stateSwitch_done();
			} else {
				//done with step; initiate next step
				steppingTask.subscribeToStop(&steppingTaskStopSubscription);
				steppingTask.startSteppingTask(1); 	//cw?
			}
		} else {
			steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		}
	}

	void stateSwitch_done() {
		positionIndex = 0; 	//reset position index
		state_e = e_done;
		advertiseStop();
	}

	void stateSwitch_waitingForSteppingTaskToIdle_toStop() {
		steppingTask.stop();
		state_e = e_waitingForSteppingTaskToIdle_toStop;
		steppingTask.subscribeToStop(&steppingTaskStopSubscription);
	}

	void waitingForSteppingTaskToIdle_toStop() {
		if (!steppingTask.isActive()) {
			//stopped
			state_e = e_idle;
			advertiseStop();
		} else {
			steppingTask.subscribeToStop(&steppingTaskStopSubscription);
		}
	}

	/// -- End of State functions -- ///
};
