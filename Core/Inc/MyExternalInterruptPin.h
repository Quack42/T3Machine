/*
 * MyExternalInterruptPin.h
 *
 *  Created on: Feb 20, 2023
 *      Author: quack
 */
#pragma once

template<typename Platform>
class ExternalInterruptPin {
public:
	void init();
    void flagInterrupt();
};

#ifdef MOCK
	#include "MockExternalInterruptPin.h"
#endif //defined(MOCK)
//#ifdef STM32
	#include "Stm32ExternalInterruptPin.h"
//#endif //defined(STM32)
