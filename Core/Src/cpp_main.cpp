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
#include "Devices.h"

#include "TimingTest0.h"

// #include "MyGPIO.h" 	//TODO: REMOVE THIS
// #include "PlatformSelection.h" 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld6; 	//blue 	//TODO: REMOVE THIS
// ld6.toggle(); 	//TODO: REMOVE THIS

TimingTest0<Platform> timingTest0(processManager, timingManager, ld3, ld6);

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
	button_filter.init(button.getValue());
	// sensor_X_filter.init(sensor_X.getValue());

	//init devices
	m415c_X.init();
	m415c_Y.init();
	drv8825_Z.init();

	////////////////////////
	// Setup connection

	button.setSubscriberFunction([](bool pinValue){button_filter.input(pinValue);});
	// button.setSubscriberFunction([](bool pinValue){timingTest0.input(pinValue);});
	button_filter.setSubscriberFunction([](bool pinValue){timingTest0.input(pinValue);});

	////////////////////////
	timingManager.start(); 	//'startRunning'

	//start infinite loop
	for(;;) {
		//wait until next task
		timingManager.waitTillNextTask();
	
		//if anything to execute: execute
		processManager.execute();

		// ld4.toggle();

		// HAL_Delay(10000.f);
	}
}
