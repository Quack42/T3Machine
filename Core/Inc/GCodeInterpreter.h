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
	CyclicBuffer<GCodeCommand, kCommandQueueSize> queue;
	GCodeCommand activeCommand;
	bool hasActiveCommand = false;
	ProcessRequest executeCommandProcessRequest;

	std::function<void(const char*, uint32_t)> channelToHostFunction;

	Subscriber controlledDeviceIdleSubscription;

	bool relativePositioning = false;
	bool inputIsMM = true;
	enum FeedRateMode_e {
		e_feedRateInverseTime,
		e_feedRateUnitsPerMinute,
		e_feedRateUnitsPerRevolution,
	} feedRate = e_feedRateUnitsPerMinute;
	bool pauzed = false;
	GCode_e lastTxCommand = e_T0; 	//Shows which tool was selected last
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
			debug("GCI.input:!queue.isEmpty()\n", sizeof("GCI.input:!queue.isEmpty()\n")-1);
		} else {
			debug("GCI.input:queue.isEmpty()\n", sizeof("GCI.input:queue.isEmpty()\n")-1);
		}

		// TODO: Acknowledge commands.
		// https://reprap.org/wiki/G-code#Buffering
		// https://github.com/MarlinFirmware/Marlin/blob/bugfix-2.1.x/Marlin/src/gcode/queue.cpp
		// 	look at: "GCodeQueue::RingBuffer::ok_to_send()"
		// Acknowledge: "ok"
		return true;
	}

	void unpauze() {
		this->pauzed = false;
		processManager.requestProcess(executeCommandProcessRequest);
	}

private:
	void executeCommand() {
		debugToHost("eC\n");
		if (!hasActiveCommand && !queue.isEmpty() && !pauzed) {
			debugToHost("Exec\n");

			// Check if the buffer is full before we read from it.
			bool queueIsFull = !queue.hasSpace();

			// Get a command to execute.
			activeCommand = queue.read();

			// Make sure the host can keep the buffer full
			if (queueIsFull && queue.hasSpace()) { 	//NOTE: queue.hasSpace() is redundant after queue.read().
				// If the queue was full, but isn't anymore: let the host know.
				sendToHost("ok\n");
			}

			// Print which command we're executing
			{
				char buffer[100] = "Executing: ";
				GCode_e code = activeCommand.getCode();
				int codeNumber = 0;
				int i = sizeof("Executing: ")-1;
				if (code >= e_GMin && code <= e_GMax) {
					// Write the GCode identifier character into the buffer.
					buffer[i] = 'G';
					i++;
					// Compute the codeNumber.
					codeNumber = code - e_GBase;
				} else if (code >= e_TMin && code <= e_TMax) {
					// Write the GCode identifier character into the buffer.
					buffer[i] = 'T';
					i++;
					// Compute the codeNumber.
					codeNumber = code - e_TBase;
				} else if (code >= e_MMin && code <= e_MMax) {
					// Write the GCode identifier character into the buffer.
					buffer[i] = 'M';
					i++;
					// Compute the codeNumber.
					codeNumber = code - e_MBase;
				}

				// Write the codeNumber value into the buffer.
				i += int_to_str(&buffer[i], sizeof(buffer)-i-1, codeNumber);

				// Add space
				buffer[i] = ' ';
				i++;

				// Handle parameters
				GCodeParameter * parameterList = activeCommand.getParameterList();
				unsigned int parameterListLength = activeCommand.getParameterListLength();
				for (unsigned int i2=0; i2 < parameterListLength; i2++) {
					buffer[i] = parameterList[i2].getIdentifier();
					i++;
					i += float_to_str(&buffer[i], sizeof(buffer)-i-1, parameterList[i2].getValue());
					buffer[i] = ' ';
					i++;
				}

				buffer[i] = '\n';
				i++;

				debug(buffer, i);
			}

			// Initiate command.
			switch(activeCommand.getCode()) {
				case e_G0:
				case e_G1:
					debugToHost("G0-1\n");
					hasActiveCommand = true;
					g0_1(activeCommand);
					break;
				// case e_G2:
				// 	debugToHost("G2\n");
				// 	hasActiveCommand = true;
				// 	g2(activeCommand);
				// 	break;
				case e_G21:
					debugToHost("G21\n");
					g21(activeCommand);
					break;
				case e_G28:
					debugToHost("G28\n");
					hasActiveCommand = true;
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
				case e_G94:
					debugToHost("G94\n");
					g94(activeCommand);
					break;
				case e_M0:
				case e_M1:
					debugToHost("M0-1\n");
					m0_1(activeCommand);
					break;
				case e_M3:
					debugToHost("M3\n");
					m3(activeCommand);
					break;
				case e_M5:
					debugToHost("-M5\n");
					m5(activeCommand);
					break;
				case e_M6:
					debugToHost("M6\n");
					m6(activeCommand);
					break;
				case e_T0:
				case e_T1:
				case e_T2:
				case e_T3:
				case e_T4:
				case e_T5:
				case e_T6:
					debugToHost("T0-6\n");
					t0_6(activeCommand);
					break;
				default:
					//TODO: Indicate something went wrong.
					return;
			}

			if(!hasActiveCommand && !queue.isEmpty() && !pauzed) {
				processManager.requestProcess(executeCommandProcessRequest);
			}
		}
	}

	void t3MachineIdleCallback() {
		// Re-register
		controlledDevice.subscribeToIdle(&controlledDeviceIdleSubscription);

		debug("GCI:T3Idle\n", sizeof("GCI:T3Idle\n")-1);

		// Execution done.
		switch (activeCommand.getCode()) {
			case e_G0:
			case e_G1:
			// case e_G2:
			case e_G28:
				hasActiveCommand = false;
				break;
			default:
				break;
		}

		// Initiate excecution.
		if (!queue.isEmpty()) {
			processManager.requestProcess(executeCommandProcessRequest);
			debug("GCI:!queue.isEmpty()\n", sizeof("GCI:!queue.isEmpty()\n")-1);
		} else {
			debug("GCI:queue.isEmpty()\n", sizeof("GCI:queue.isEmpty()\n")-1);
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

	bool g02(GCodeCommand & command) { 	//TODO: this function
		//X, Y (,Z) are the coordinates to move to
		//I, J, (,K) are the coordinates of the rotational point (incremental or absoluteness depends on another command; I hear incremental is the most common one)
		return false;
	}

	bool g21(GCodeCommand & command) {
		setInputAsMM();
		hasActiveCommand = false;
		return true;
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

	bool g94(GCodeCommand & command) {
		// "Feedrate is units[mm||inches]/minute"
		this->feedRate = e_feedRateUnitsPerMinute;
		return true;
	}

	bool m0_1(GCodeCommand & command) {
		pauzePrompt();
		return true;
	}

	bool m3(GCodeCommand & command) {
		// https://marlinfw.org/docs/gcode/M003.html
		// Spindle CW / Laser On
		//TODO: Do I need to consider lastTxCommand ?
		controlledDevice.setDrillState(true);
		return true;
	}

	bool m5(GCodeCommand & command) {
		// https://marlinfw.org/docs/gcode/M005.html
		// Spindle / Laser Off
		//TODO: Do I need to consider lastTxCommand ?
		controlledDevice.setDrillState(false);
		return true;
	}

	bool m6(GCodeCommand & command) {
		//Stop the drill
		controlledDevice.setDrillState(false);
		//Pauze and prompt user til continue
		pauzePrompt();
		//TODO: notify user of tool change
		char buffer[] = "Expecting tool change:[TX]\n";
		buffer[24] = '0' + (lastTxCommand - e_TBase);
		DEBUG_VCOM(buffer);
		return true;
	}

	bool t0_6(GCodeCommand & command) {
		lastTxCommand = command.getCode();

		char buffer[] = "Select tool:[TX]\n";
		buffer[14] = '0' + (command.getCode() - e_TBase);
		DEBUG_VCOM(buffer);
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

	void setInputAsMM() {
		this->inputIsMM = true;
	}

	void pauzePrompt() {
		this->pauzed = true;
		DEBUG_VCOM("Pauzed\n");
	}
};


// https://www.youtube.com/watch?v=4SZ0PtO79Mg
