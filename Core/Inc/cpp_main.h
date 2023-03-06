/*
 * main.hpp
 *
 *  Created on: Feb 25, 2023
 *      Author: quack
 */

#ifndef INC_CPP_MAIN_H_
#define INC_CPP_MAIN_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"


#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


EXTERNC void cpp_main(void);
EXTERNC void CPP_HAL_GPIO_EXTI_Callback(uint16_t pin);
EXTERNC void CPP_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim);
#undef EXTERNC

#endif /* INC_CPP_MAIN_H_ */
