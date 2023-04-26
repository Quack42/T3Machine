#pragma once

#include "ProcessManager.h"
#include "CyclicBuffer.h"
#include "ProcessRequest.h"
#include "SubscriberLL.h"

#include "GCodeCommand.h"
#include "GCodeParameter.h"
#include "GCodeParse.h"

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
	CyclicBuffer<GCodeCommand, 10> queue;
	GCodeCommand activeCommand;
	bool hasActiveCommand = false;
	ProcessRequest executeCommandProcessRequest;

	std::function<void(const char*, uint32_t)> channelToHostFunction;

	Subscriber controlledDeviceIdleSubscription;


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

	void setChannelToHost(std::function<void(const char*, uint32_t)> channelToHostFunction) {
		this->channelToHostFunction = channelToHostFunction;
	}

	//line : a gcode line without line-ending characters
	bool input(const char * line, uint32_t lineLength) {
		if (!queue.hasSpace()) {
			sendToHost("1\n", 2);
			return false;
		}

		GCodeParse gCodeParse(line, lineLength);
		// Parse.
		if (!gCodeParse.parse()) {
			//TODO: indicate something went wrong.
			sendToHost("2\n", 2);
			return false;
		}

		// Generate command.
		GCodeCommand cmd;
		if (!gCodeParse.generateCommand(&cmd)) {
			//TODO: indicate something went wrong.
			sendToHost("3\n", 2);
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

		// // Execute.
		// switch(cmd.getCode()) {
		// 	case e_G0:
		// 		g0(cmd);
		// 		break;
		// 	default:
		// 		//TODO: Indicate something went wrong.
		// 		return false;
		// }


		// TODO: Acknowledge commands.
		// https://reprap.org/wiki/G-code#Buffering
		// https://github.com/MarlinFirmware/Marlin/blob/bugfix-2.1.x/Marlin/src/gcode/queue.cpp
		// 	look at: "GCodeQueue::RingBuffer::ok_to_send()"
		// Acknowledge: "ok"
		return true;
	}

private:
	void executeCommand() {
		// TODO: If ready to execute.
		if (!hasActiveCommand && !queue.isEmpty()) {
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
					g0_1(activeCommand);
					break;
				default:
					//TODO: Indicate something went wrong.
					return;
			}
		}
	}

	void t3MachineIdleCallback() {
		// Execution done.
		hasActiveCommand = false;

		// Initiate excecution.
		if (!queue.isEmpty()) {
			processManager.requestProcess(executeCommandProcessRequest);
		}
	}

	bool g0_1(GCodeCommand & cmd) {
		// https://marlinfw.org/docs/gcode/G000-G001.html
		float movementE=0.0f;
		float movementX=0.0f;
		float movementY=0.0f;
		float movementZ=0.0f;

		float powerS=0.0f;
		bool hasPowerS = false;

		float rateF=0.0f;
		bool hasRateF = false;

		GCodeParameter * parameterList = cmd.getParameterList();
		for (unsigned int i=0; i < cmd.getParameterListLength(); i++) {
			GCodeParameter & parameter = parameterList[i];
			switch(parameter.getIdentifier()) {
				case 'X':
					movementX = parameter.getValue();
					break;
				case 'Y':
					movementY = parameter.getValue();
					break;
				case 'Z':
					movementZ = parameter.getValue();
					break;
				case 'E':
					movementE = parameter.getValue();
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

		// if (hasPowerS) { 	//TODO: implement
		// 	// Laser feature
		// 	controlledDevice.setPower(powerS);
		// }

		// if (hasRateF) { 	//TODO: implement
		// 	controlledDevice.setMaximumMovementSpeed(rateF);
		// }

		return controlledDevice.moveAbsolute(movementX, movementY, movementZ);
	}

	void sendToHost(const char * msg, uint32_t msgLength) {
		if (channelToHostFunction) {
			channelToHostFunction(msg, msgLength);
		}
	}
	void sendToHost(const char * nullTerminatedMessage) {
		sendToHost(nullTerminatedMessage, strlen(nullTerminatedMessage));
	}
};
