#pragma once

#include "MyGPIO.h"

//srd-05vdc-sl-c: Power off: default state. Powered but "IN" undriven: Floats to default state. Powered and high on IN: retaining default state.

template<typename Platform>
class Relay {
private:
	OutputPin<Platform> & controlPin;
public:
	Relay(OutputPin<Platform> & controlPin) :
			controlPin(controlPin)
	{

	}

	void on() {
		controlPin.high();
	}

	void off() {
		controlPin.low();
	}
};
