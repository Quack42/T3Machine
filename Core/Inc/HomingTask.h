#pragma once

#include "HomingAxisTask.h"

template<typename DriverX, typename DriverY, typename DriverZ, typename SensorX, typename SensorY, typename SensorZ, typename Platform>
class HomingTask {
private:
	//References
	HomingAxisTask<DriverX, SensorX, Platform> & homingAxisTask_X;
	HomingAxisTask<DriverY, SensorY, Platform> & homingAxisTask_Y;
	HomingAxisTask<DriverZ, SensorZ, Platform> & homingAxisTask_Z;
	
	//Variables
	enum {
		e_idle,
		e_homingX,
		e_homingY,
		e_homingZ,
		e_done
	} state_e;
public:
	HomingTask(	HomingAxisTask<DriverX, SensorX, Platform> & homingAxisTask_X,
				HomingAxisTask<DriverY, SensorY, Platform> & homingAxisTask_Y,
				HomingAxisTask<DriverZ, SensorZ, Platform> & homingAxisTask_Z) :
			homingAxisTask_X(homingAxisTask_X),
			homingAxisTask_Y(homingAxisTask_Y),
			homingAxisTask_Z(homingAxisTask_Z),
			state_e(e_idle)
	{
	}

	void start() {
		stateSwitch_toHomingX();
	}

	void tick(float timePassed) {
		switch(state_e) {
			case e_idle:
				//do nothing
				break;
			case e_homingX:
				state_homingX();
				break;
			case e_homingY:
				state_homingY();
				break;
			case e_homingZ:
				state_homingZ();
				break;
			case e_done:
				//do nothing
				break;
			default:
				//do nothing
				break;
		}
	}

private:
	void stateSwitch_toHomingX() {
		state_e = e_homingX;
		homingAxisTask_X.start();
	}

	void state_homingX() {
		if (homingAxisTask_X.isDone()) {
			stateSwitch_toHomingY();
		}
	}

	void stateSwitch_toHomingY() {
		state_e = e_homingY;
		homingAxisTask_Y.start();
	}

	void state_homingY() {
		if (homingAxisTask_Y.isDone()) {
			stateSwitch_toHomingZ();
		}
	}

	void stateSwitch_toHomingZ() {
		state_e = e_homingZ;
		homingAxisTask_Z.start();
	}

	void state_homingZ() {
		if (homingAxisTask_Z.isDone()) {
			stateSwitch_toDone();
		}
	}

	void stateSwitch_toDone() {
		state_e = e_done;
	}

};
