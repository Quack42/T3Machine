/*#pragma once

#include "MyGPIO.h"

enum ActiveLevel {
	ActiveLow,
	ActiveHigh
};

template<typename Platform, ActiveLevel A>
class PinControlOut {
private:
	PinData<Platform> & pin;
public:
	PinControlOut(PinData<Platform> & pin) :
			pin(pin)
	{

	}

	void activate();
	void deactivate();
	bool isActive();
};

template<typename Platform, ActiveLevel A>
inline void PinControlOut<Platform, ActiveLow>::activate() {
	pin.low();
}

template<typename Platform>
inline void PinControlOut<Platform, ActiveHigh>::activate() {
	pin.high();
}

template<typename Platform>
inline void PinControlOut<Platform, ActiveLow>::deactivate() {
	pin.high();
}
template<typename Platform>
inline void PinControlOut<Platform, ActiveHigh>::deactivate() {
	pin.low();
}

template<typename Platform>
inline bool PinControlOut<Platform, ActiveLow>::isActive() {
	return !pin.read();
}
template<typename Platform>
inline bool PinControlOut<Platform, ActiveHigh>::isActive() {
	return pin.read();
}
*/

