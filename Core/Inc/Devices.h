#pragma once

#include "PlatformSelection.h"
#include "Pins.h"

//process control
#include "Timing.h"
#include "Timers.h"
#include "TimingManager.h"
#include "ProcessManager.h"

//entities
#include "M415C.h"
#include "DRV8825.h"
#include "MyGPIO.h"
#include "MyExternalInterruptPin.h"
#include "T3Machine.h"

// //HAL Handles
// extern TIM_HandleTypeDef htim10;

//platform
extern ProcessManager<Platform> processManager;
extern TimingManager<Platform> timingManager;//(htim10);


extern PinData<Platform> ld3; 	//orange
extern PinData<Platform> ld4; 	//green
extern PinData<Platform> ld5; 	//red
extern PinData<Platform> ld6; 	//blue

extern PinData<Platform> stepPin_X;//(GPIOE, GPIO_PIN_0);
extern PinData<Platform> stepPin_Y;//(GPIOE, GPIO_PIN_13);
extern PinData<Platform> stepPin_Z;//(GPIOE, GPIO_PIN_7);
extern PinData<Platform> directionPin_X;//(GPIOE, GPIO_PIN_6);
extern PinData<Platform> directionPin_Y;//(GPIOE, GPIO_PIN_11);
extern PinData<Platform> directionPin_Z;//(GPIOE, GPIO_PIN_9);


//interrupt pins
extern ExternalInterruptPin<Platform> button;//(processManager, button_pinIdentifier, false);
extern ExternalInterruptPin<Platform> sensor_X;//(processManager, sensor_X_pinIdentifier, true);
extern ExternalInterruptPin<Platform> sensor_Y;//(processManager, sensor_Y_pinIdentifier, true);
extern ExternalInterruptPin<Platform> sensor_Z;//(processManager, sensor_Z_pinIdentifier, true);

//devices
extern M415C<Platform> m415c_X;//(stepPin_X, directionPin_X);
extern M415C<Platform> m415c_Y;//(stepPin_Y, directionPin_Y);
extern DRV8825_MovementControl<Platform> drv8825_Z;//(stepPin_Z, directionPin_Z);
extern T3Machine<
	Platform,
	M415C<Platform>,
	M415C<Platform>,
	DRV8825_MovementControl<Platform>,
	ExternalInterruptPin<Platform>,
	ExternalInterruptPin<Platform>,
	ExternalInterruptPin<Platform>
	> t3Machine;//(
	// 	timingManager,
	// 	m415c_X,
	// 	m415c_Y,
	// 	drv8825_Z,
	// 	sensor_X,
	// 	sensor_Y,
	// 	sensor_Z
	// );