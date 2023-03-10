#pragma once

#include "MyGPIO.h"


template<typename Platform>
class M415C {
private:
	PinData<Platform> & stepPin;
	PinData<Platform> & directionPin;
public:
	M415C(PinData<Platform> & stepPin, PinData<Platform> & directionPin) :
			stepPin(stepPin),
			directionPin(directionPin)
	{

	}

	void init() {
		// stepPin.setValue(true);
		// directionPin.setValue(true);
	}

	void setStepPin(const bool & pinValue) {
		stepPin.setValue(!pinValue); 	//TODO: figure out if the step takes place on low->high or high->low
	}

	void setDirectionPin(const bool & pinValue) {
		directionPin.setValue(pinValue);
	}
};
