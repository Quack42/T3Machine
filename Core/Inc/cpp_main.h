/*
 * main.hpp
 *
 *  Created on: Feb 25, 2023
 *      Author: quack
 */

#pragma once

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
EXTERNC void CPP_HAL_GPIO_EXTI_Callback(uint16_t pin); 	//TODO: move this elsewhere; keep cpp_main platform agnostic
EXTERNC void CPP_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim); 	 	//TODO: move this elsewhere; keep cpp_main platform agnostic
EXTERNC void CPP_USB_CDC_DataReceived(uint8_t * data, uint32_t dataLength); 	 	//TODO: move this elsewhere; keep cpp_main platform agnostic
EXTERNC void CPP_USB_CDC_DataTransmissionComplete(); 	 	//TODO: move this elsewhere; keep cpp_main platform agnostic

#undef EXTERNC
