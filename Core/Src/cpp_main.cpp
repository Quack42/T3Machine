/*
 * main.cpp
 *
 *  Created on: Feb 25, 2023
 *      Author: quack
 */
#include "PlatformSelection.h"

#include "cpp_main.h"
#include "main.h"

#include "Pins.h"


// //process control
// #include "Timing.h"
// #include "TimingManager.h"
// #include "ProcessManager.h"

// //entities
// #include "M415C.h"
// #include "DRV8825.h"
// #include "MyGPIO.h"
// #include "MyExternalInterruptPin.h"
// #include "T3Machine.h"

// //tasks
// #include "SteppingTask.h"
// #include "HomingTask.h"

#include "Devices.h"

// //HAL Handles
// extern TIM_HandleTypeDef htim10;

// //platform
// TimingManager<Platform> timingManager(htim10);
// ProcessManager<Platform> processManager;

// //pins
// PinData<Platform> ld3(LD3_GPIO_Port, LD3_Pin);
// PinData<Platform> ld4(LD4_GPIO_Port, LD4_Pin);
// PinData<Platform> ld5(LD5_GPIO_Port, LD5_Pin);
// PinData<Platform> ld6(LD6_GPIO_Port, LD6_Pin);

// PinData<Platform> stepPin_X(GPIOE, GPIO_PIN_0);
// PinData<Platform> directionPin_X(GPIOE, GPIO_PIN_6);
// PinData<Platform> stepPin_Y(GPIOE, GPIO_PIN_13);
// PinData<Platform> directionPin_Y(GPIOE, GPIO_PIN_11);
// PinData<Platform> stepPin_Z(GPIOE, GPIO_PIN_7);
// PinData<Platform> directionPin_Z(GPIOE, GPIO_PIN_9);

// ExternalInterruptPin_PlatformData<Platform> button_platformData(B1_GPIO_Port, B1_Pin);
// ExternalInterruptPin_PlatformData<Platform> sensor_X_platformData(GPIOE, GPIO_PIN_4);
// ExternalInterruptPin_PlatformData<Platform> sensor_Y_platformData(GPIOA, GPIO_PIN_1);
// ExternalInterruptPin_PlatformData<Platform> sensor_Z_platformData(GPIOE, GPIO_PIN_2);
// ExternalInterruptPin<Platform> button(processManager, button_platformData, false);
// ExternalInterruptPin<Platform> sensor_X(processManager, sensor_X_platformData, true);
// ExternalInterruptPin<Platform> sensor_Y(processManager, sensor_Y_platformData, true);
// ExternalInterruptPin<Platform> sensor_Z(processManager, sensor_Z_platformData, true);



// //devices
// M415C<Platform> m415c_X(stepPin_X, directionPin_X);
// M415C<Platform> m415c_Y(stepPin_Y, directionPin_Y);
// DRV8825_MovementControl<Platform> drv8825_Z(stepPin_Z, directionPin_Z);
// T3Machine<
// 	Platform,
// 	M415C<Platform>,
// 	M415C<Platform>,
// 	DRV8825_MovementControl<Platform>,
// 	ExternalInterruptPin<Platform>,
// 	ExternalInterruptPin<Platform>,
// 	ExternalInterruptPin<Platform>
// 	> t3Machine(
// 		timingManager,
// 		m415c_X,
// 		m415c_Y,
// 		drv8825_Z,
// 		sensor_X,
// 		sensor_Y,
// 		sensor_Z
// 	);

void cpp_main(void) {
	//init platform
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

		// HAL_Delay(10000.f); 	//TODO: use timer

		//update time sensitive tasks
		// steppingTask.tick(timeSlept);

		t3Machine.tick(timeSlept);
		// processManager.execute();
	}
}
