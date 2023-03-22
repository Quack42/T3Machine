#pragma once

#include "MyGPIO.h"


template<typename Platform>
class M415C {
private:
	OutputPin<Platform> & stepPin;
	OutputPin<Platform> & directionPin;
public:
	M415C(OutputPin<Platform> & stepPin, OutputPin<Platform> & directionPin) :
			stepPin(stepPin),
			directionPin(directionPin)
	{

	}

	void init() {
		
	}

	void setStepPin(const bool & pinValue) {
		stepPin.setValue(!pinValue); 	//TODO: figure out if the step takes place on low->high or high->low
	}

	void setDirectionPin(const bool & pinValue) {
		directionPin.setValue(pinValue);
	}
};
