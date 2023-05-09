#pragma once

#include "ProcessManager.h"
#include "CyclicBuffer.h"
#include "ProcessRequest.h"
#include "SubscriberLL.h"

#include "GCodeCommand.h"
#include "GCodeParameter.h"
#include "GCodeParse.h"

#include "logging.h"

#include <cstdint>
#include <cstring>
#include <functional>

template<typename Platform, typename ControlledDevice>
class GCodeInterpreter {
private:
	/// References
	ProcessManager<Platform> & processManager;
	ControlledDevice & controlledDevice;

	/// Components
	CyclicBuffer<GCodeCommand, 2> queue;
	GCodeCommand activeCommand;
	bool hasActiveCommand = false;
	ProcessRequest executeCommandProcessRequest;

	std::function<void(const char*, uint32_t)> channelToHostFunction;

	Subscriber controlledDeviceIdleSubscription;

	bool relativePositioning = false;
public:
	GCodeInterpreter(	ProcessManager<Platform> & processManager,
						ControlledDevice & controlledDevice) :
			// References
			processManager(processManager),
			controlledDevice(controlledDevice),
			// Components
			executeCommandProcessRequest(std::bind(&GCodeInterpreter<Platform, ControlledDevice>::executeCommand, this)),
			controlledDeviceIdleSubscription(std::bind(&GCodeInterpreter<Platform, ControlledDevice>::t3MachineIdleCallback, this))
	{
	}

	void init() {
		controlledDevice.subscribeToIdle(&controlledDeviceIdleSubscription);
	}

	//TODO: reset input per file

	void setChannelToHost(std::function<void(const char*, uint32_t)> channelToHostFunction) {
		this->channelToHostFunction = channelToHostFunction;
	}

	//line : a gcode line without line-ending characters
	bool input(const char * line, uint32_t lineLength) {
		if (queue.isFull()) {
			return false;
		}

		GCodeParse gCodeParse(line, lineLength);
		// Parse.
		if (!gCodeParse.parse()) {
			//TODO: indicate something went wrong.
			return false;
		}

		// Generate command.
		GCodeCommand cmd;
		if (!gCodeParse.generateCommand(&cmd)) {
			//TODO: indicate something went wrong.
			return false;
		}

		// Store in queue.
		queue.write(cmd);

		// Acknowledge command if there is space for another.
		if (queue.hasSpace()) {
			sendToHost("ok\n");
		}

		// Initiate excecution.
		if (!queue.isEmpty()) {
			processManager.requestProcess(executeCommandProcessRequest);
		}

		// TODO: Acknowledge commands.
		// https://reprap.org/wiki/G-code#Buffering
		// https://github.com/MarlinFirmware/Marlin/blob/bugfix-2.1.x/Marlin/src/gcode/queue.cpp
		// 	look at: "GCodeQueue::RingBuffer::ok_to_send()"
		// Acknowledge: "ok"
		return true;
	}

private:
	void executeCommand() {
		debugToHost("eC\n");
		if (!hasActiveCommand && !queue.isEmpty()) {
			debugToHost("Exec\n");

			// Check if the buffer is full before we read from it.
			bool queueIsFull = !queue.hasSpace();

			// Get a command to execute.
			activeCommand = queue.read();
			hasActiveCommand = true;

			// Make sure the host can keep the buffer full
			if (queueIsFull && queue.hasSpace()) { 	//NOTE: queue.hasSpace() is redundant after queue.read().
				// If the queue was full, but isn't anymore: let the host know.
				sendToHost("ok\n");
			}


			// Initiate command.
			switch(activeCommand.getCode()) {
				case e_G0:
				case e_G1:
					debugToHost("G0-1\n");
					g0_1(activeCommand);
					break;
				case e_G28:
					debugToHost("G28\n");
					g28(activeCommand);
					break;
				case e_G90:
					debugToHost("G90\n");
					g90(activeCommand);
					break;
				case e_G91:
					debugToHost("G91\n");
					g91(activeCommand);
					break;
				case e_G92:
					debugToHost("G92\n");
					g92(activeCommand);
					break;
				default:
					//TODO: Indicate something went wrong.
					return;
			}
		}
	}

	void t3MachineIdleCallback() {
		// Re-register
		controlledDevice.subscribeToIdle(&controlledDeviceIdleSubscription);

		// DEBUG_VCOM("GCI:T3Idle\n");
		debug("GCI:T3Idle\n", sizeof("GCI:T3Idle\n")-1);
		// Execution done.
		hasActiveCommand = false;

		// Initiate excecution.
		if (!queue.isEmpty()) {
			processManager.requestProcess(executeCommandProcessRequest);
		}
	}

	bool g0_1(GCodeCommand & command) {
		// https://marlinfw.org/docs/gcode/G000-G001.html
		bool hasMovementE = false;
		float movementE = 0.0f;
		bool hasMovementX = false;
		float movementX = 0.0f;
		bool hasMovementY = false;
		float movementY = 0.0f;
		bool hasMovementZ = false;
		float movementZ = 0.0f;

		float powerS = 0.0f;
		bool hasPowerS = false;

		float rateF = 0.0f;
		bool hasRateF = false;

		GCodeParameter * parameterList = command.getParameterList();
		for (unsigned int i=0; i < command.getParameterListLength(); i++) {
			GCodeParameter & parameter = parameterList[i];
			switch(parameter.getIdentifier()) {
				case 'E':
					movementE = parameter.getValue();
					hasMovementE = true;
					break;
				case 'X':
					movementX = parameter.getValue();
					hasMovementX = true;
					break;
				case 'Y':
					movementY = parameter.getValue();
					hasMovementY = true;
					break;
				case 'Z':
					movementZ = parameter.getValue();
					hasMovementZ = true;
					break;
				case 'S':
					powerS = parameter.getValue();
					hasPowerS = true;
					break;
				case 'F':
					rateF = parameter.getValue();
					hasRateF = true;
					break;
				default:
					// Unrecognized parameter.
					return false;
			}
		}

		UNUSED(hasMovementE);
		UNUSED(movementE);
		UNUSED(powerS);
		UNUSED(hasPowerS);
		UNUSED(rateF);
		UNUSED(hasRateF);

		// if (hasPowerS) { 	//TODO: implement
		// 	// Laser feature
		// 	controlledDevice.setPower(powerS);
		// }

		// if (hasRateF) { 	//TODO: implement
		// 	controlledDevice.setMaximumMovementSpeed(rateF);
		// }

		if (relativePositioning) {
			return controlledDevice.moveRelativeLinear(hasMovementX, movementX, hasMovementY, movementY, hasMovementZ, movementZ);
		} else {
			return controlledDevice.moveAbsoluteLinear(hasMovementX, movementX, hasMovementY, movementY, hasMovementZ, movementZ);
		}
	}

	bool g28(GCodeCommand & command) {
		// https://marlinfw.org/docs/gcode/G028.html
		bool flagL = false; 	// Flag to restore bed leveling state after homing. (default true).
		bool flagO = false; 	// Flag to skip homing if the position is already trusted.

		float distanceToRaiseNozzleR = 0.0f; 	// The distance to raise the nozzle before homing.
		bool hasDistanceToRaiseNozzleR = false;

		bool flagX = false; 	// Flag to home the X axis.
		bool flagY = false; 	// Flag to home the Y axis.
		bool flagZ = false; 	// Flag to home the Z axis.

		GCodeParameter * parameterList = command.getParameterList();
		for (unsigned int i=0; i < command.getParameterListLength(); i++) {
			GCodeParameter & parameter = parameterList[i];
			switch(parameter.getIdentifier()) {
				case 'X':
					flagX = true;
					break;
				case 'Y':
					flagY = true;
					break;
				case 'Z':
					flagZ = true;
					break;
				case 'L':
					flagL = true;
					break;
				case 'O':
					flagO = true;
					break;
				case 'R':
					distanceToRaiseNozzleR = parameter.getValue();
					hasDistanceToRaiseNozzleR = true;
					break;
				default:
					// Unrecognized parameter.
					return false;
			}
		}

		UNUSED(flagL);
		UNUSED(flagO);

		UNUSED(distanceToRaiseNozzleR);
		UNUSED(hasDistanceToRaiseNozzleR);


		return controlledDevice.homingSequence(flagX, flagY, flagZ);
	}

	bool g90(GCodeCommand & command) {
		// https://marlinfw.org/docs/gcode/G090.html
		relativePositioning = false;
		return true;
	}

	bool g91(GCodeCommand & command) {
		// https://marlinfw.org/docs/gcode/G091.html
		relativePositioning = true;
		return true;
	}


	bool g92(GCodeCommand & command) {
		float newEPosition = 0.f; 		// New extruder position.
		bool hasNewEPosition = false;

		float newXPosition = 0.f; 		// New X axis position.
		bool hasNewXPosition = false;

		float newYPosition = 0.f; 		// New Y axis position.
		bool hasNewYPosition = false;

		float newZPosition = 0.f; 		// New Z axis position.
		bool hasNewZPosition = false;

		GCodeParameter * parameterList = command.getParameterList();
		for (unsigned int i=0; i < command.getParameterListLength(); i++) {
			GCodeParameter & parameter = parameterList[i];
			switch(parameter.getIdentifier()) {
				case 'E':
					hasNewEPosition = true;
					newEPosition = parameter.getValue();
					break;
				case 'X':
					hasNewXPosition = true;
					newXPosition = parameter.getValue();
					break;
				case 'Y':
					hasNewYPosition = true;
					newYPosition = parameter.getValue();
					break;
				case 'Z':
					hasNewZPosition = true;
					newZPosition = parameter.getValue();
					break;
				default:
					// Unrecognized parameter.
					return false;
			}
		}

		if (hasNewEPosition) {
			controlledDevice.setNewEPosition(newEPosition);
		}
		if (hasNewXPosition) {
			controlledDevice.setNewXPosition(newXPosition);
		}
		if (hasNewYPosition) {
			controlledDevice.setNewYPosition(newYPosition);
		}
		if (hasNewZPosition) {
			controlledDevice.setNewZPosition(newZPosition);
		}
		return true;
	}

	void _sendToHost(const char * msg, uint32_t msgLength) {
		if (channelToHostFunction) {
			channelToHostFunction(msg, msgLength);
		}
	}
	void sendToHost(const char * msg, uint32_t msgLength) {
		if (kDebugMode) {
			_sendToHost("PROT:", /*strlen("PROT:")*/ 5);
		}
		_sendToHost(msg, msgLength);
	}
	void sendToHost(const char * nullTerminatedMessage) {
		sendToHost(nullTerminatedMessage, strlen(nullTerminatedMessage));
	}

	void debugToHost(const char * msg, uint32_t msgLength) {
		// debug(msg, msgLength);
		if (kDebugMode) {
			_sendToHost("DEBUG", /*strlen("DEBUG")*/ 5);
			_sendToHost(msg, msgLength);
		}
	}
	void debugToHost(const char * nullTerminatedMessage) {
		debugToHost(nullTerminatedMessage, strlen(nullTerminatedMessage));
	}
};
