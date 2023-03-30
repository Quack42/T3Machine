/*
 * T3Machine.h
 *
 *  Created on: Feb 26, 2023
 *      Author: quack
 */

#pragma once

//tasks
#include "SteppingTask.h"
#include "HomingTask.h"

template<typename Platform, typename DriverX, typename DriverY, typename DriverZ>
class T3Machine {
private:
	//References
	ProcessManager<Platform> & processManager; 	//TODO: check if used here.
	TimingManager<Platform> & timingManager; 	//TODO: check if used here.
	DriverX & driverX;
	DriverY & driverY;
	DriverZ & driverZ;

	//Variables
	enum {
		e_idle,
		e_homingState
	} state_e = e_idle;

	int positionX = 0; 	//in steps
	int positionY = 0; 	//in steps
	int positionZ = 0; 	//in steps

	//tasks
	SteppingTask<DriverX, Platform> steppingTask_X;
	SteppingTask<DriverY, Platform> steppingTask_Y;
	SteppingTask<DriverZ, Platform> steppingTask_Z;
	HomingAxisTask<DriverX, Platform> homingAxisTask_X;
	HomingAxisTask<DriverY, Platform> homingAxisTask_Y;
	HomingAxisTask<DriverZ, Platform> homingAxisTask_Z;
	HomingTask<DriverX, DriverY, DriverZ, Platform> homingTask;

public:
	T3Machine(	ProcessManager<Platform> & processManager,
				TimingManager<Platform> & timingManager,
				DriverX & driverX,
				DriverY & driverY,
				DriverZ & driverZ
	) :
			processManager(processManager),
			timingManager(timingManager),
			driverX(driverX),
			driverY(driverY),
			driverZ(driverZ),
			state_e(e_idle),
			steppingTask_X(processManager, timingManager, driverX, positionX),
			steppingTask_Y(processManager, timingManager, driverY, positionY),
			steppingTask_Z(processManager, timingManager, driverZ, positionZ),
			homingAxisTask_X(processManager, steppingTask_X, positionX),
			homingAxisTask_Y(processManager, steppingTask_Y, positionY),
			homingAxisTask_Z(processManager, steppingTask_Z, positionZ),
			homingTask(homingAxisTask_X, homingAxisTask_Y, homingAxisTask_Z)
	{

	}

	void startHoming() {
		homingTask.start();
	}

	void startMoving() {
		steppingTask_Z.startSteppingTask(10000);
	}

	void input_sensorX(bool sensorXValue) {
		homingTask.input_sensorX(sensorXValue);
	}

	void input_sensorY(bool sensorXValue) {
		homingTask.input_sensorY(sensorXValue);
	}

	void input_sensorZ(bool sensorXValue) {
		homingTask.input_sensorZ(sensorXValue);
	}

private:
	void emergencyStop() {
		//TODO: halt behaviors
		steppingTask_X.stop();
		steppingTask_Y.stop();
		steppingTask_Z.stop();
		homingTask.stop();

		//TODO: indicate issue to user
	}
};
