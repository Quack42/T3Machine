/*
 * Stm32ExternalInterruptPin.h
 *
 *  Created on: Feb 20, 2023
 *      Author: quack
 */

#pragma once

//https://stm32f4-discovery.net/2014/08/stm32f4-external-interrupts-tutorial/
// Don't use multiple interrupt pins on the same 'line' (i.e. PortA_Pin0 uses the same line as PortB_Pin0)

template<>
class ExternalInterruptPin<Stm32F407Platform> {
private:
	GPIO_TypeDef * port;
	const uint16_t pin;
	const bool activeLow;
	bool flaggedInterrupt = false;
public:
	ExternalInterruptPin(GPIO_TypeDef * port, const uint16_t & pin, const bool & activeLow) :
			port(port),
			pin(pin),
			activeLow(activeLow)
	{
	}

	const uint16_t getPin() const {
		return pin;
	}

	bool getValue() {
		return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET) != activeLow;
	}

	void init() {
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; 	//TODO: make this configurable
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(port, &GPIO_InitStruct);
		//HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);

		switch(pin) {
		case GPIO_PIN_0:
			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
			break;
		case GPIO_PIN_1:
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);
			break;
		case GPIO_PIN_2:
			HAL_NVIC_EnableIRQ(EXTI2_IRQn);
			break;
		case GPIO_PIN_3:
			HAL_NVIC_EnableIRQ(EXTI3_IRQn);
			break;
		case GPIO_PIN_4:
			HAL_NVIC_EnableIRQ(EXTI4_IRQn);
			break;
		case GPIO_PIN_5:
		case GPIO_PIN_6:
		case GPIO_PIN_7:
		case GPIO_PIN_8:
		case GPIO_PIN_9:
			HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
		case GPIO_PIN_10:
		case GPIO_PIN_11:
		case GPIO_PIN_12:
		case GPIO_PIN_13:
		case GPIO_PIN_14:
		case GPIO_PIN_15:
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		}
	}

    void flagInterrupt() {
		flaggedInterrupt = true;
    }

    bool isInterruptFlagged() {
    	bool ret = flaggedInterrupt;
    	//TODO: if an interrupt occurs here it can be missed. (flaggedInterrupt has already been read; <interrupt> flaggedInterrupt is set to true in flagInterrupt(); </interrupt> flaggedInterrupt is set back to false)
    	flaggedInterrupt = false;
    	return ret;
    }
};
