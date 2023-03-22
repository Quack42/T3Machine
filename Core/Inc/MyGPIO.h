#pragma once

//TODO: Refactor file to OutputPin.h

#include "PinIdentifier.h"

template<typename Platform>
class OutputPin {
private:
	PinIdentifier<Platform> & pinIdentifier;
public:
	OutputPin(PinIdentifier<Platform> & pinIdentifier) :
			pinIdentifier(pinIdentifier)
	{

	}

	void init(const bool & initialState = false);
	void toggle();
	void setValue(const bool & value);


	void low() {
		setValue(false);
	}
	void high() {
		setValue(true);
	}
};
