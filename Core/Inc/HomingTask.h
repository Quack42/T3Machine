#pragma once

#include "HomingAxisTask.h"

#include "MyGPIO.h" 	//TODO: REMOVE THIS
#include "PlatformSelection.h" 	//TODO: REMOVE THIS
extern OutputPin<Platform> ld6; 	//blue 	//TODO: REMOVE THIS
extern OutputPin<Platform> ld5; 	//red 	//TODO: REMOVE THIS
extern OutputPin<Platform> ld4; 	//grn 	//TODO: REMOVE THIS
extern OutputPin<Platform> ld3; 	//ora 	//TODO: REMOVE THIS


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
	bool homeX;
	bool homeY;
	bool homeZ;

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

	void init() {
		homingAxisTask_X.subscribeToStop(&homingAxisTaskXStoppedSubscription);
		homingAxisTask_Y.subscribeToStop(&homingAxisTaskYStoppedSubscription);
		homingAxisTask_Z.subscribeToStop(&homingAxisTaskZStoppedSubscription);
	}

	bool start(bool homeX, bool homeY, bool homeZ) {
		if (state_e == e_idle || state_e == e_done) {
			this->homeX = homeX;
			this->homeY = homeY;
			this->homeZ = homeZ;
			stateSwitch_toHomingX();
			return true;
		} else {
			return false;
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
		ld4.high();
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
		if (homeX) {
			state_e = e_homingX;
			ld3.high();
			ld5.low();
			ld6.low();
			homingAxisTask_X.start();
		} else {
			stateSwitch_toHomingY();
		}
	}

	void state_homingX() {
		if (homingAxisTask_X.isDone()) {
			stateSwitch_toHomingY();
		}
	}

	void stateSwitch_toHomingY() {
		if (homeY) {
			state_e = e_homingY;
			ld3.low();
			ld5.high();
			ld6.low();
			homingAxisTask_Y.start();
		} else {
			stateSwitch_toHomingZ();
		}
	}

	void state_homingY() {
		if (homingAxisTask_Y.isDone()) {
			stateSwitch_toHomingZ();
		}
	}

	void stateSwitch_toHomingZ() {
		if (homeZ) {
			state_e = e_homingZ;
			ld3.low();
			ld5.low();
			ld6.high();
			homingAxisTask_Z.start();
		} else {
			stateSwitch_toDone();
		}
	}

	void state_homingZ() {
		if (homingAxisTask_Z.isDone()) {
			stateSwitch_toDone();
		}
	}

	void stateSwitch_toDone() {
		state_e = e_done;
		ld3.low();
		ld5.low();
		ld6.low();
	}

	/// -- End of State functions -- ///

};
