#pragma once

#include "PinIdentifier.h"

template<typename Platform>
class PinData { 	//TODO: refactor to OutputPin
private:
	PinIdentifier<Platform> & pinIdentifier;
public:
	PinData(PinIdentifier<Platform> & pinIdentifier) :
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

#ifdef MOCK
	#include "MockGPIO.h"
#endif //defined(MOCK)
//#ifdef STM32
//	#include "Stm32GPIO.h"
//#endif //defined(STM32)
