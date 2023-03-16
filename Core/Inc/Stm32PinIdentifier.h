#pragma once

#include "PinIdentifier.h"
#include "stm32f4xx_hal.h"
#include "Stm32F407Platform.h"

template<>
class PinIdentifier<Stm32F407Platform> {
private:
	GPIO_TypeDef * port;
	const uint16_t pin;
public:
	PinIdentifier(GPIO_TypeDef * port, const uint16_t pin) :
			port(port),
			pin(pin)
	{

	}

	GPIO_TypeDef * getPort() const {
		return port;
	}

	const uint16_t & getPin() const {
		return pin;
	}
};
