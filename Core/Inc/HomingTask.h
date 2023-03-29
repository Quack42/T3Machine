#pragma once

#include "HomingAxisTask.h"

template<typename DriverX, typename DriverY, typename DriverZ, typename Platform>
class HomingTask {
private:
	// References
	HomingAxisTask<DriverX, Platform> & homingAxisTask_X;
	HomingAxisTask<DriverY, Platform> & homingAxisTask_Y;
	HomingAxisTask<DriverZ, Platform> & homingAxisTask_Z;
	
	// Components
	Subscriber homingAxisTaskXStoppedSubscription;
	Subscriber homingAxisTaskYStoppedSubscription;
	Subscriber homingAxisTaskZStoppedSubscription;

	// Variables
	enum {
		e_idle,
		e_homingX,
		e_homingY,
		e_homingZ,
		e_done
	} state_e;

public:
	HomingTask(	HomingAxisTask<DriverX, Platform> & homingAxisTask_X,
				HomingAxisTask<DriverY, Platform> & homingAxisTask_Y,
				HomingAxisTask<DriverZ, Platform> & homingAxisTask_Z) :
			homingAxisTask_X(homingAxisTask_X),
			homingAxisTask_Y(homingAxisTask_Y),
			homingAxisTask_Z(homingAxisTask_Z),
			homingAxisTaskXStoppedSubscription(std::bind(&HomingTask::homingAxisTaskXStoppedCallback, this)),
			homingAxisTaskYStoppedSubscription(std::bind(&HomingTask::homingAxisTaskYStoppedCallback, this)),
			homingAxisTaskZStoppedSubscription(std::bind(&HomingTask::homingAxisTaskZStoppedCallback, this)),
			state_e(e_idle)
	{
	}

	void start() {
		if (state_e == e_idle || state_e == e_done) {
			stateSwitch_toHomingX();
		} else {
			//TODO: fill this.
		}
	}

	void input_sensorX(bool sensorXValue) {
		homingAxisTask_X.input_sensor(sensorXValue);
	}
	void input_sensorY(bool sensorYValue) {
		homingAxisTask_Y.input_sensor(sensorYValue);
	}
	void input_sensorZ(bool sensorZValue) {
		homingAxisTask_Z.input_sensor(sensorZValue);
	}

private:
	void homingAxisTaskXStoppedCallback() {
		if (state_e == e_homingX) {
			state_homingX();
		}
	}
	void homingAxisTaskYStoppedCallback() {
		if (state_e == e_homingY) {
			state_homingY();
		}
	}
	void homingAxisTaskZStoppedCallback() {
		if (state_e == e_homingZ) {
			state_homingZ();
		}
	}

	/// -- State functions -- ///


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

	/// -- End of State functions -- ///

};
