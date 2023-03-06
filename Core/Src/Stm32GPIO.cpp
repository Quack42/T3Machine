#include "Stm32GPIO.h"
//#ifdef STM32

void PinData<Stm32F407Platform>::toggle(){
	HAL_GPIO_TogglePin(port, pin);
}

void PinData<Stm32F407Platform>::setValue(const bool & value) {
	HAL_GPIO_WritePin(port, pin, (value) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

//#endif //defined(STM32)
