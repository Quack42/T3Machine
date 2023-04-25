#pragma once

#include "GCodeParameter.h"
#include "ConfigurableConstants.h"

enum GCode_e {
	e_GBase = 0,
	// -----
	e_G0 = e_GBase + 0,
	// -----
	e_INVALID = -1
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
