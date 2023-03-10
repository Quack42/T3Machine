#pragma once
//#ifdef STM32

#include "MyGPIO.h"

#include "Stm32F407Platform.h"

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif
	#include "stm32f4xx_hal.h"
#ifdef __cplusplus
}
#endif


template<>
class PinData<Stm32F407Platform> {
private:
	GPIO_TypeDef * port;
	const uint16_t pin;
public:
	PinData(GPIO_TypeDef * port, const uint16_t & pin) :
			port(port),
			pin(pin)
	{
	}

	void init(GPIO_PinState initialState = GPIO_PIN_RESET) {
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_WritePin(port, pin, initialState);
		HAL_GPIO_Init(port, &GPIO_InitStruct);
	}

	void toggle();
	void setValue(const bool & value);
	void low() {
		setValue(false);
	}
	void high() {
		setValue(true);
	}
};

//#endif //defined(STM32)
