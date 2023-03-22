#include "Stm32PinIdentifier.h"
#include "Stm32F407Platform.h"

#include "main.h" 	//for STM32CubeIDE generated pin names

#include "stm32f4xx_hal.h"

//#ifdef STM32

PinIdentifier<Stm32F407Platform> ld3_pinIdentifier(LD3_GPIO_Port, LD3_Pin);
PinIdentifier<Stm32F407Platform> ld4_pinIdentifier(LD4_GPIO_Port, LD4_Pin);
PinIdentifier<Stm32F407Platform> ld5_pinIdentifier(LD5_GPIO_Port, LD5_Pin);
PinIdentifier<Stm32F407Platform> ld6_pinIdentifier(LD6_GPIO_Port, LD6_Pin);

PinIdentifier<Stm32F407Platform> stepPin_X_pinIdentifier(GPIOE, GPIO_PIN_0);
PinIdentifier<Stm32F407Platform> stepPin_Y_pinIdentifier(GPIOE, GPIO_PIN_13);
PinIdentifier<Stm32F407Platform> stepPin_Z_pinIdentifier(GPIOE, GPIO_PIN_7);
PinIdentifier<Stm32F407Platform> directionPin_X_pinIdentifier(GPIOE, GPIO_PIN_6);
PinIdentifier<Stm32F407Platform> directionPin_Y_pinIdentifier(GPIOE, GPIO_PIN_11);
PinIdentifier<Stm32F407Platform> directionPin_Z_pinIdentifier(GPIOE, GPIO_PIN_9);

PinIdentifier<Stm32F407Platform> button_pinIdentifier(B1_GPIO_Port, B1_Pin);
PinIdentifier<Stm32F407Platform> sensor_X_pinIdentifier(GPIOE, GPIO_PIN_4);
PinIdentifier<Stm32F407Platform> sensor_Y_pinIdentifier(GPIOA, GPIO_PIN_1);
PinIdentifier<Stm32F407Platform> sensor_Z_pinIdentifier(GPIOE, GPIO_PIN_2);

//#endif //STM32
