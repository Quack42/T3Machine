/*
 * T3Machine.h
 *
 *  Created on: Feb 26, 2023
 *      Author: quack
 */

#pragma once

// Tasks
#include "SteppingTask.h"
#include "HomingTask.h"

// Components
#include "Vector3.h"

template<typename Platform, typename DriverX, typename DriverY, typename DriverZ>
class T3Machine {
private:
	/// References
	ProcessManager<Platform> & processManager; 	//TODO: check if used here.
	TimingManager<Platform> & timingManager; 	//TODO: check if used here.
	DriverX & driverX;
	DriverY & driverY;
	DriverZ & driverZ;

	/// Variables
	enum {
		e_idle,
		e_homingState
	} state_e = e_idle;

	// Current location, in steps
	Vector3i stepOffset;

	// The desired location in offset from 0; doesn't always perfectly match 'step*' variables, so we need to remember it in case of relative positioning.
	Vector3f targetPosition;

	/// Components
	// Tasks
	SteppingTask<DriverX, Platform> steppingTask_X;
	SteppingTask<DriverY, Platform> steppingTask_Y;
	SteppingTask<DriverZ, Platform> steppingTask_Z;
	HomingAxisTask<DriverX, Platform> homingAxisTask_X;
	HomingAxisTask<DriverY, Platform> homingAxisTask_Y;
	HomingAxisTask<DriverZ, Platform> homingAxisTask_Z;
	HomingTask<DriverX, DriverY, DriverZ, Platform> homingTask;

	Subscriber steppingTaskXStopSubscription;
	Subscriber steppingTaskYStopSubscription;
	Subscriber steppingTaskZStopSubscription;
	
	Subscriber * idleSubscriptionList = nullptr;


public:
	T3Machine(	ProcessManager<Platform> & processManager,
				TimingManager<Platform> & timingManager,
				DriverX & driverX,
				DriverY & driverY,
				DriverZ & driverZ,
				Timer<Platform> & steppingTaskXTimer,
				Timer<Platform> & steppingTaskYTimer,
				Timer<Platform> & steppingTaskZTimer
	) :
			processManager(processManager),
			timingManager(timingManager),
			driverX(driverX),
			driverY(driverY),
			driverZ(driverZ),
			state_e(e_idle),
			stepOffset(),
			targetPosition(),
			steppingTask_X(processManager, timingManager, steppingTaskXTimer, driverX, stepOffset.getRefX()),
			steppingTask_Y(processManager, timingManager, steppingTaskYTimer, driverY, stepOffset.getRefY()),
			steppingTask_Z(processManager, timingManager, steppingTaskZTimer, driverZ, stepOffset.getRefZ()),
			homingAxisTask_X(processManager, steppingTask_X, stepOffset.getRefX()),
			homingAxisTask_Y(processManager, steppingTask_Y, stepOffset.getRefY()),
			homingAxisTask_Z(processManager, steppingTask_Z, stepOffset.getRefZ()),
			homingTask(homingAxisTask_X, homingAxisTask_Y, homingAxisTask_Z),
			steppingTaskXStopSubscription(std::bind(&T3Machine<Platform, DriverX, DriverY, DriverZ>::steppingTaskXStopped, this)),
			steppingTaskYStopSubscription(std::bind(&T3Machine<Platform, DriverX, DriverY, DriverZ>::steppingTaskYStopped, this)),
			steppingTaskZStopSubscription(std::bind(&T3Machine<Platform, DriverX, DriverY, DriverZ>::steppingTaskZStopped, this))
	{
	}

	void subscribeToIdle(Subscriber * sub) {
		Subscriber::addSubscription(sub, &idleSubscriptionList);
	}

	void init() {
		steppingTask_X.init();
		steppingTask_Y.init();
		steppingTask_Z.init();
		homingTask.init();
	}

	void startHoming() {
		homingTask.start();
	}

	void startMoving() {
		steppingTask_X.startSteppingTask(10000);
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

	bool moveAbsolute(float x, float y, float z) {
		//TODO: Throw a tantrum if target position is out of bounds.
		//TODO: Throw a tantrum if steppingTasks aren't done.
		
		// Set target position; needs to be done first.
		targetPosition = Vector3f(x,y,z);

		// Set target position
		Vector3i stepsToTake = calculateTargetStepOffset() - stepOffset;

		//TODO: set steppingTask* time
		// 50-50 split time per step.
		// setStepHighTime
		// setStepLowTime

		// Subscribe to stop of stepping tasks.
		steppingTask_X.subscribeToStop(&steppingTaskXStopSubscription);
		steppingTask_Y.subscribeToStop(&steppingTaskYStopSubscription);
		steppingTask_Z.subscribeToStop(&steppingTaskZStopSubscription);

		// Start stepping.
		steppingTask_X.startSteppingTask(stepsToTake.x);
		steppingTask_Y.startSteppingTask(stepsToTake.y);
		steppingTask_Z.startSteppingTask(stepsToTake.z);

		return true;
	}

	bool isIdle() {
		return (!steppingTask_X.isActive() && !steppingTask_Y.isActive() && !steppingTask_Z.isActive());
	}

private:

	void advertiseIdle() {
		/// Advertise
		while (idleSubscriptionList != nullptr) {
			//first remove first from list
			Subscriber * current = idleSubscriptionList;
			idleSubscriptionList = idleSubscriptionList->getNext();
			//then add to process managers to-call list.
			processManager.requestProcess(current->getProcessRequest());
		}
	}

	void emergencyStop() {
		//TODO: halt behaviors
		steppingTask_X.stop();
		steppingTask_Y.stop();
		steppingTask_Z.stop();
		homingTask.stop();

		//TODO: indicate issue to user
	}


	Vector3f calculateCurrentPosition() const {
		return Vector3f( kMMPerTickXAxis * stepOffset.x,
						 kMMPerTickYAxis * stepOffset.y,
						 kMMPerTickZAxis * stepOffset.z );
	}

	Vector3i calculateTargetStepOffset() const {
		//TODO: make sure bounds of the working area are taken into account.
		return Vector3i( targetPosition.x / kMMPerTickXAxis + 0.5f,
						 targetPosition.y / kMMPerTickYAxis + 0.5f,
						 targetPosition.z / kMMPerTickZAxis + 0.5f);
	}

	void steppingTaskStopped() {
		if (isIdle()) {
			advertiseIdle();
		}
	}

	void steppingTaskXStopped() {
		steppingTaskStopped();
	}

	void steppingTaskYStopped() {
		steppingTaskStopped();
	}

	void steppingTaskZStopped() {
		steppingTaskStopped();
	}
};
