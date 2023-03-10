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
#include "DRV8825.h"
#include "MyGPIO.h"
#include "MyExternalInterruptPin.h"
#include "T3Machine.h"

//tasks
#include "SteppingTask.h"
#include "HomingTask.h"

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

PinData<Platform> stepPin_X(GPIOE, GPIO_PIN_0);
PinData<Platform> directionPin_X(GPIOE, GPIO_PIN_6);
PinData<Platform> stepPin_Y(GPIOE, GPIO_PIN_13);
PinData<Platform> directionPin_Y(GPIOE, GPIO_PIN_11);
PinData<Platform> stepPin_Z(GPIOE, GPIO_PIN_7);
PinData<Platform> directionPin_Z(GPIOE, GPIO_PIN_9);


ExternalInterruptPin<Platform> sensor_X(GPIOE, GPIO_PIN_4, true);
ExternalInterruptPin<Platform> sensor_Y(GPIOA, GPIO_PIN_1, true);
ExternalInterruptPin<Platform> sensor_Z(GPIOE, GPIO_PIN_2, true);

//devices
M415C<Platform> m415c_X(stepPin_X, directionPin_X);
M415C<Platform> m415c_Y(stepPin_Y, directionPin_Y);
DRV8825_MovementControl<Platform> drv8825_Z(stepPin_Z, directionPin_Z);
T3Machine<
	Platform,
	M415C<Platform>,
	M415C<Platform>,
	DRV8825_MovementControl<Platform>,
	ExternalInterruptPin<Platform>,
	ExternalInterruptPin<Platform>,
	ExternalInterruptPin<Platform>
	> t3Machine(
		timingManager,
		m415c_X,
		m415c_Y,
		drv8825_Z,
		sensor_X,
		sensor_Y,
		sensor_Z
	);

void cpp_main(void) {
	//init platform
	Platform::Init();
	timingManager.init();

	//init pins
	stepPin_X.init(GPIO_PIN_SET);
	directionPin_X.init(GPIO_PIN_SET);
	stepPin_Y.init(GPIO_PIN_SET);
	directionPin_Y.init(GPIO_PIN_SET);
	stepPin_Z.init(GPIO_PIN_SET);
	directionPin_Z.init(GPIO_PIN_SET);
	ld3.init();
	ld4.init();
	ld5.init();
	ld6.init();
	button.init();
	sensor_X.init();
	sensor_Y.init();
	sensor_Z.init();

	//init devices
	m415c_X.init();
	m415c_Y.init();
	drv8825_Z.init();


	// ld5.setValue(true);
	for(;;) {
		//sleep
		float timeSlept = timingManager.sleep();
		ld4.toggle();

		//update time sensitive tasks
		// steppingTask.tick(timeSlept);

		t3Machine.tick(timeSlept);
	}
}

void CPP_HAL_GPIO_EXTI_Callback(uint16_t pin) {
	if (pin == sensor_X.getPin()) {
		sensor_X.flagInterrupt();
		// timingManager.wakeup();
		

		// steppingTask.stop();
		//for fun; keep LD5 (red) LED matched to sensor_X value
		ld5.setValue(sensor_X.getValue());
	}
	if (pin == sensor_Y.getPin()) {
		sensor_Y.flagInterrupt();
		//for fun; keep LD3 (orange) LED matched to sensor_Y value
		ld3.setValue(sensor_Y.getValue());
	}
	if (pin == sensor_Z.getPin()) {
		sensor_Z.flagInterrupt();
		//for fun; keep LD6 (color: blue?) LED matched to sensor_Z value
		ld6.setValue(sensor_Z.getValue());
	}

	if (pin == button.getPin() && !button.getValue()) {
		// steppingTask.startSteppingTask(1000);
		t3Machine.startHoming();
		timingManager.wakeup();
	}
}

void CPP_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim) {
	if(htim == &htim10) {
		// timingManager.awake();
		// ld6.toggle();
	}
}
