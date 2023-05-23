#pragma once

#include "GCodeParameter.h"
#include "ConfigurableConstants.h"

enum GCode_e {
	e_GBase = 0,
	e_TBase = 100,
	e_MBase = 200,
	// -----
	e_G0 = e_GBase + 0, 		// Non-extrude linear move. 				https://marlinfw.org/docs/gcode/G000-G001.html
	e_G1 = e_GBase + 1, 		// Extrude Linear move. 					https://marlinfw.org/docs/gcode/G000-G001.html
	e_G21 = e_GBase + 21, 		// Millimeter Units. 						https://marlinfw.org/docs/gcode/G021.html
	e_G28 = e_GBase + 28, 		// Auto home. 								https://marlinfw.org/docs/gcode/G028.html
	e_G90 = e_GBase + 90, 		// Absolute Positioning. 					https://marlinfw.org/docs/gcode/G090.html
	e_G91 = e_GBase + 91, 		// Relative Positioning. 					https://marlinfw.org/docs/gcode/G091.html
	e_G92 = e_GBase + 92, 		// Set Position. 							https://marlinfw.org/docs/gcode/G092.html
	e_G94 = e_GBase + 94, 		// Units per Minute Mode. 					https://linuxcnc.org/docs/html/gcode/g-code.html#gcode:g93-g94-g95
	// -----
	e_T0 = e_TBase + 0, 		// Select Tool 0. 							https://marlinfw.org/docs/gcode/T001-T002.html
	e_T1 = e_TBase + 1, 		// Select Tool 1. 							https://marlinfw.org/docs/gcode/T001-T002.html
	e_T2 = e_TBase + 2, 		// Select Tool 2. 							https://marlinfw.org/docs/gcode/T001-T002.html
	e_T3 = e_TBase + 3, 		// Select Tool 3. 							https://marlinfw.org/docs/gcode/T001-T002.html
	e_T4 = e_TBase + 4, 		// Select Tool 4. 							https://marlinfw.org/docs/gcode/T001-T002.html
	e_T5 = e_TBase + 5, 		// Select Tool 5. 							https://marlinfw.org/docs/gcode/T001-T002.html
	e_T6 = e_TBase + 6, 		// Select Tool 6. 							https://marlinfw.org/docs/gcode/T001-T002.html
	// -----
	e_M0 = e_MBase + 0, 		// Unconditional pauze (prompt user). 		https://marlinfw.org/docs/gcode/M000-M001.html
	e_M1 = e_MBase + 1, 		// Unconditional pauze (prompt user). 		https://marlinfw.org/docs/gcode/M000-M001.html
	e_M3 = e_MBase + 3, 		// Spindle CW / Laser On. 					https://marlinfw.org/docs/gcode/M003.html
	e_M5 = e_MBase + 5, 		// Spindle / Laser Off. 					https://marlinfw.org/docs/gcode/M005.html
	e_M6 = e_MBase + 6, 		// Prompt user for Tool change. 			https://linuxcnc.org/docs/html/gcode/m-code.html#mcode:m6
	// -----
	e_INVALID = -1,

	e_GMin = e_G0,
	e_GMax = e_G94,

	e_TMin = e_T0,
	e_TMax = e_T6,

	e_MMin = e_M0,
	e_MMax = e_M6,
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
