#pragma once

template<typename Platform>
class PinData {
public:
	void toggle() {}
	void setValue(const bool & value) {}
};

#ifdef MOCK
	#include "MockGPIO.h"
#endif //defined(MOCK)
//#ifdef STM32
	#include "Stm32GPIO.h"
//#endif //defined(STM32)
