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

template<typename Platform, typename DriverX, typename DriverY, typename DriverZ, typename SensorX, typename SensorY, typename SensorZ>
class T3Machine {
private:
	//References
	TimingManager<Platform> & timingManager; 	//TODO: check if used here.
	DriverX & driverX;
	DriverY & driverY;
	DriverZ & driverZ;
	SensorX & sensorX;
	SensorY & sensorY;
	SensorZ & sensorZ;



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
	HomingAxisTask<DriverX, SensorX, Platform> homingAxisTask_X;
	HomingAxisTask<DriverY, SensorY, Platform> homingAxisTask_Y;
	HomingAxisTask<DriverZ, SensorZ, Platform> homingAxisTask_Z;
	HomingTask<DriverX, DriverY, DriverZ, SensorX, SensorY, SensorZ, Platform> homingTask;

public:
	T3Machine(	TimingManager<Platform> & timingManager,
				DriverX & driverX,
				DriverY & driverY,
				DriverZ & driverZ,
				SensorX & sensorX,
				SensorY & sensorY,
				SensorZ & sensorZ
	) :
			timingManager(timingManager),
			driverX(driverX),
			driverY(driverY),
			driverZ(driverZ),
			sensorX(sensorX),
			sensorY(sensorY),
			sensorZ(sensorZ),
			state_e(e_idle),
			steppingTask_X(timingManager, driverX, positionX),
			steppingTask_Y(timingManager, driverY, positionY),
			steppingTask_Z(timingManager, driverZ, positionZ),
			homingAxisTask_X(sensorX, steppingTask_X, positionX),
			homingAxisTask_Y(sensorY, steppingTask_Y, positionY),
			homingAxisTask_Z(sensorZ, steppingTask_Z, positionZ),
			homingTask(homingAxisTask_X, homingAxisTask_Y, homingAxisTask_Z)
	{

	}

	void startHoming() {
		homingTask.start();
	}

	void tick(const float & timeSlept) {
		//tick tasks (note that the order matters; lowest-level first)
		steppingTask_X.tick(timeSlept);
		steppingTask_Y.tick(timeSlept);
		steppingTask_Z.tick(timeSlept);
		homingAxisTask_X.tick(timeSlept);
		homingAxisTask_Y.tick(timeSlept);
		homingAxisTask_Z.tick(timeSlept);
		homingTask.tick(timeSlept);

		// if (sensorX.getValue()) {
		// 	if (!homingTask.isActive()){
		// 		//shouldn't trigger sensors outside of homing task
		// 		emergencyStop();
		// 	}
		// }
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
