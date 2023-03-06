/*
 * main.cpp
 *
 *  Created on: Feb 25, 2023
 *      Author: quack
 */

#include <cpp_main.h>
#include "main.h"
#include "Stm32F407Platform.h"

typedef Stm32F407Platform Platform;

//process control
#include "Timing.h"
#include "TimingManager.h"

//entities
#include "M415C.h"
#include "MyGPIO.h"
#include "MyExternalInterruptPin.h"
#include "T3Machine.h"

//tasks
#include "SteppingTask.h"

//HAL Handles
extern TIM_HandleTypeDef htim10;

//platform
TimingManager<Platform> timingManager(htim10);

//pins
PinData<Platform> ld3(LD3_GPIO_Port, LD3_Pin);
PinData<Platform> ld4(LD4_GPIO_Port, LD4_Pin);
PinData<Platform> ld5(LD5_GPIO_Port, LD5_Pin);
PinData<Platform> ld6(LD6_GPIO_Port, LD6_Pin);
ExternalInterruptPin<Platform> button(B1_GPIO_Port, B1_Pin, false);

PinData<Platform> stepXPin(GPIOE, GPIO_PIN_1);
PinData<Platform> directionXPin(GPIOE, GPIO_PIN_0);


ExternalInterruptPin<Platform> sensorX(GPIOE, GPIO_PIN_2, true);
ExternalInterruptPin<Platform> sensorY(GPIOA, GPIO_PIN_3, true);
ExternalInterruptPin<Platform> sensorZ(GPIOA, GPIO_PIN_5, true);

//devices
M415C<Platform> m415c_X(stepXPin, directionXPin);
T3Machine<Platform, M415C<Platform>, ExternalInterruptPin<Platform>> t3Machine(m415c_X, button);

//tasks
SteppingTask<M415C<Platform>, Platform> steppingTask(m415c_X, timingManager);

void cpp_main(void) {
	//init platform
	Platform::Init();
	timingManager.init();

	//init pins
	stepXPin.init();
	directionXPin.init();
	ld3.init();
	ld4.init();
	ld5.init();
	ld6.init();
	button.init();
	sensorX.init();

	//init devices
	//..

	//init tasks


	ld5.setValue(true);
	for(;;) {
		//sleep
		//ld4.setValue(true);
		float timeSlept = timingManager.sleep();
		//ld4.setValue(false);
		ld4.toggle();

		//update time sensitive tasks
		steppingTask.tick(timeSlept);
	}
}

void CPP_HAL_GPIO_EXTI_Callback(uint16_t pin) {
	if (pin == sensorX.getPin()) {
		sensorX.flagInterrupt();
		steppingTask.stop();
		ld5.setValue(sensorX.getValue());
	}
	if (pin == button.getPin()) {
		steppingTask.startSteppingTask(1000);
		timingManager.wakeup();
		ld5.setValue(true);
	}
}

void CPP_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim) {
	if(htim == &htim10) {
		//timingManager.awake();
		ld6.toggle();
	}
}
