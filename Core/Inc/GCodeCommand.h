#pragma once

#include "GCodeParameter.h"
#include "ConfigurableConstants.h"

enum GCode_e {
	e_GBase = 0,
	// -----
	e_G0 = e_GBase + 0, 		// Non-extrude linear move. 	https://marlinfw.org/docs/gcode/G000-G001.html
	e_G1 = e_GBase + 1, 		// Extrude Linear move. 		https://marlinfw.org/docs/gcode/G000-G001.html
	e_G28 = e_GBase + 28, 		// Auto home. 					https://marlinfw.org/docs/gcode/G028.html
	e_G90 = e_GBase + 90, 		// Absolute Positioning. 		https://marlinfw.org/docs/gcode/G090.html
	e_G91 = e_GBase + 91, 		// Relative Positioning. 		https://marlinfw.org/docs/gcode/G091.html
	e_G92 = e_GBase + 92, 		// Set Position. 				https://marlinfw.org/docs/gcode/G092.html
	// -----
	e_INVALID = -1,
	e_GMin = e_G0,
	e_GMax = e_G92
};


class GCodeCommand {
private:
	GCode_e code = e_INVALID;
	GCodeParameter parameterList[kMaximumNumberOfParameters];
	unsigned int parameterListLength = 0;

public:
	GCodeCommand() :
			code(e_INVALID),
			parameterListLength(0)
	{
	}

	void setCode(GCode_e code) {
		this->code = code;
	}

	GCode_e getCode() const {
		return code;
	}

	bool addParameter(GCodeParameter parameter) {
		if (parameterListLength == kMaximumNumberOfParameters) {
			// Doesn't fit in the parameter list.
			return false;
		}
		parameterList[parameterListLength] = parameter;
		parameterListLength++;

		return true;
	}

	unsigned int getParameterListLength() const {
		return parameterListLength;
	}

	GCodeParameter * getParameterList() {
		return &parameterList[0];
	}
};
