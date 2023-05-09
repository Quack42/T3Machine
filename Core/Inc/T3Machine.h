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
	Vector3i absoluteStepPosition;
	int absoluteStepPositionExtruder;

	Vector3f offset;
	float offsetExtruder;

	// The desired location in offset from 0; doesn't always perfectly match 'step*' variables, so we need to remember it in case of relative positioning.
	Vector3f targetPosition;
	float targetExtruder;

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
			absoluteStepPosition(0,0,0),
			offset(0,0,0),
			offsetExtruder(0),
			targetPosition(0,0,0),
			targetExtruder(0),
			steppingTask_X('X', processManager, timingManager, steppingTaskXTimer, driverX, absoluteStepPosition.getRefX()),
			steppingTask_Y('Y', processManager, timingManager, steppingTaskYTimer, driverY, absoluteStepPosition.getRefY()),
			steppingTask_Z('Z', processManager, timingManager, steppingTaskZTimer, driverZ, absoluteStepPosition.getRefZ()),
			homingAxisTask_X(processManager, steppingTask_X, absoluteStepPosition.getRefX()),
			homingAxisTask_Y(processManager, steppingTask_Y, absoluteStepPosition.getRefY()),
			homingAxisTask_Z(processManager, steppingTask_Z, absoluteStepPosition.getRefZ()),
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
		homingTask.start(true, true, false); 	//TODO: homeZ should be true, but currently the motor driver is burnt out.
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

	// GCode called functions
	void setRelativePositioning(bool relativePositioning) {
		this->relativePositioning = relativePositioning;
	}

	void setNewEPosition(float newEPosition) {
		offsetExtruder = newEPosition;
	}
	void setNewXPosition(float newXPosition) {
		offset.x = newXPosition;
	}
	void setNewYPosition(float newYPosition) {
		offset.y = newYPosition;
	}
	void setNewZPosition(float newZPosition) {
		offset.z = newZPosition;
	}


	bool homingSequence(bool homeX, bool homeY, bool homeZ) {
		return homingTask.start(homeX, homeY, homeZ);
	}

	bool moveRelativeLinear(bool moveX, float x, bool moveY, float y, bool moveZ, float z) {
		DEBUG_VCOM("REL-MOVE\n");
		// Calculate absolute position based on relative coordinates.
		Vector3f newTargetPosition = targetPosition + Vector3f(x,y,z);
		
		// Call moveAbsolute with those coordinates.
		return moveAbsoluteLinear(moveX, newTargetPosition.x, moveY, newTargetPosition.y, moveZ, newTargetPosition.z);
	}

	bool moveAbsoluteLinear(bool moveX, float x, bool moveY, float y, bool moveZ, float z) {
		//TODO: Throw a tantrum if target position is out of bounds. Also consider moveX/Y/Z booleans.
		//TODO: Throw a tantrum if steppingTasks aren't done.
		
		// Set target position; needs to be done first.
		targetPosition = Vector3f(
				moveX ? x : targetPosition.x,
				moveY ? y : targetPosition.y,
				moveZ ? z : targetPosition.z);

		// Set target position.
		Vector3i stepsToTake = calculateTargetStepOffset() - absoluteStepPosition;

		{
			int stepsToTakeX = stepsToTake.x;
			bool negative = false;
			if (stepsToTakeX < 0) {
				stepsToTakeX *= -1;
				negative = true;
			}
			char buffer[] = "T3:stepsToTake.x:[      0][M]\n";
			int i = 0;
			while (stepsToTakeX) {
				buffer[24-i] = '0' + (stepsToTakeX % 10);
				stepsToTakeX /= 10;
				i++;
			}
			if (negative) {
				buffer[24-i] = '-';
			}
			buffer[27] = '0' + moveX;
			debug(buffer, sizeof("T3:stepsToTake.x:[       ][M]\n")-1);
		}
		{
			int stepsToTakeY = stepsToTake.y;
			bool negative = false;
			if (stepsToTakeY < 0) {
				stepsToTakeY *= -1;
				negative = true;
			}
			char buffer[] = "T3:stepsToTake.y:[      0][M]\n";
			int i = 0;
			while (stepsToTakeY) {
				buffer[24-i] = '0' + (stepsToTakeY % 10);
				stepsToTakeY /= 10;
				i++;
			}
			if (negative) {
				buffer[24-i] = '-';
			}
			buffer[27] = '0' + moveY;
			debug(buffer, sizeof("T3:stepsToTake.y:[       ][M]\n")-1);
		}
		{
			int stepsToTakeZ = stepsToTake.z;
			bool negative = false;
			if (stepsToTakeZ < 0) {
				stepsToTakeZ *= -1;
				negative = true;
			}
			char buffer[] = "T3:stepsToTake.z:[      0][M]\n";
			int i = 0;
			while (stepsToTakeZ) {
				buffer[24-i] = '0' + (stepsToTakeZ % 10);
				stepsToTakeZ /= 10;
				i++;
			}
			if (negative) {
				buffer[24-i] = '-';
			}
			buffer[27] = '0' + moveZ;
			debug(buffer, sizeof("T3:stepsToTake.z:[       ][M]\n")-1);
		}


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
		return Vector3f( kMMPerTickXAxis * absoluteStepPosition.x,
						 kMMPerTickYAxis * absoluteStepPosition.y,
						 kMMPerTickZAxis * absoluteStepPosition.z );
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
