#pragma once

template<typename Driver, typename Sensor, typename Platform>
class HomingAxisTask {
private:
	//references
	Sensor & sensor;
	SteppingTask<Driver, Platform> & steppingTask;
	int & positionIndex;
	//variables
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
public:
	HomingAxisTask(		Sensor & sensor,
					SteppingTask<Driver, Platform> & steppingTask,
					int & positionIndex) :
			sensor(sensor),
			steppingTask(steppingTask),
			positionIndex(positionIndex),
			state_e(e_idle)
	{
	}

	void start() {
		if (steppingTask.isActive()) {
			stateSwitch_waitingForSteppingTaskToIdle_toStart();
		}
		if (state_e == e_idle || state_e == e_done) {
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

	void tick(float) {
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

private:
	void stateSwitch_waitingForSteppingTaskToIdle_toStart() {
		state_e = e_waitingForSteppingTaskToIdle_toStart;
		steppingTask.stop();
	}

	void waitingForSteppingTaskToIdle_toStart() {
		if (!steppingTask.isActive()) {
			//steppingTask stopped
			stateSwitch_towardsSensorFast();
		}
	}

	void stateSwitch_towardsSensorFast() {
		if (sensor.getValue()) {
			//already there
			stateSwitch_fromSensorFast();
		} else {
			state_e = e_towardsSensorFast;
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
			if (sensor.getValue()) {
				//done with state; go to next state
				stateSwitch_fromSensorFast();
			} else {
				//done with step; initiate next step
				steppingTask.startSteppingTask(1); 	//cw?
			}
		}
	}

	void stateSwitch_fromSensorFast() {
		state_e = e_fromSensorFast;
		steppingTask.setStepHighTime(0.5f); 	//TODO: get this magic number from a definitions list ("homing_fast_stepHighTime")
		steppingTask.setStepLowTime(0.5f); 	//TODO: get this magic number from a definitions list ("homing_fast_stepLowTime")
		steppingTask.startSteppingTask(-10); 	//ccw?
	}

	void fromSensorFast() {
		if (!steppingTask.isActive()) {
			//done stepping back
			if (sensor.getValue()) {
				//but still in sensor, so keep going
				steppingTask.startSteppingTask(-10); 	//ccw?
			} else {
				//out of sensor, go to next state
				stateSwitch_toSensorSlow();
			}
		}
	}

	void stateSwitch_toSensorSlow() {
		state_e = e_towardsSensorSlow;
		steppingTask.setStepHighTime(0.1f); 	//TODO: get this magic number from a definitions list ("homing_slow_stepHighTime")
		steppingTask.setStepLowTime(49.9f); 	//TODO: get this magic number from a definitions list ("homing_slow_stepLowTime")
		steppingTask.startSteppingTask(1); 	//ccw?		
	}
	
	void toSensorSlow() {
		if (!steppingTask.isActive()) {
			if (sensor.getValue()) {
				//done with state (found the sensor, slowly); go to done state
				stateSwitch_done();
			} else {
				//done with step; initiate next step
				steppingTask.startSteppingTask(1); 	//cw?
			}
		}		
	}

	void stateSwitch_done() {
		positionIndex = 0; 	//reset position index
		state_e = e_done;
	}

	void stateSwitch_waitingForSteppingTaskToIdle_toStop() {
		steppingTask.stop();
		state_e = e_waitingForSteppingTaskToIdle_toStop;
	}

	void waitingForSteppingTaskToIdle_toStop() {
		if (!steppingTask.isActive()) {
			//stopped
			state_e = e_idle;
		}
	}
};
