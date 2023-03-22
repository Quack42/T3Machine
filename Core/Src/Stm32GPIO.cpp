#include "MyGPIO.h"
#include "Stm32F407Platform.h"

#include "Stm32PinIdentifier.h"

#include "stm32f4xx_hal.h"

// #ifdef STM32

template<>
void OutputPin<Stm32F407Platform>::init(const bool & initialState) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = pinIdentifier.getPin();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_WritePin(pinIdentifier.getPort(), pinIdentifier.getPin(), (initialState) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_Init(pinIdentifier.getPort(), &GPIO_InitStruct);
}

template<>
void OutputPin<Stm32F407Platform>::toggle(){
	HAL_GPIO_TogglePin(pinIdentifier.getPort(), pinIdentifier.getPin());
}

template<>
void OutputPin<Stm32F407Platform>::setValue(const bool & value) {
	HAL_GPIO_WritePin(pinIdentifier.getPort(), pinIdentifier.getPin(), (value) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// #endif //STM32
