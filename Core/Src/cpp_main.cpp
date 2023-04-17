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



// #include "TimingTest0.h"
// #include "TimingTest1.h"


// TimingTest0<Platform> timingTest0(processManager, timingManager, ld3, ld6);
// TimingTest1<Platform> timingTest1(ld3, ld6, steppingTaskTimer);

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

	//init filters
	button_filter.init(button.getValue());
	sensor_X_filter.init(sensor_X.getValue());
	sensor_Y_filter.init(sensor_Y.getValue());
	sensor_Z_filter.init(sensor_Z.getValue());
	// sensor_X_filter.init(sensor_X.getValue());

	//init devices
	m415c_X.init();
	m415c_Y.init();
	drv8825_Z.init();

	t3Machine.init();

	//init timers
	steppingTaskXTimer.init();
	steppingTaskYTimer.init();
	steppingTaskZTimer.init();

	//init tests
	// timingTest1.init();

	////////////////////////
	// Setup data connections

	sensor_X.setSubscriberFunction([](bool pinValue){sensor_X_filter.input(pinValue);});
	sensor_X_filter.setSubscriberFunction([](bool pinValue){t3Machine.input_sensorX(!pinValue);}); 	//NOTE: '!' because sensor is active low

	sensor_Y.setSubscriberFunction([](bool pinValue){sensor_Y_filter.input(pinValue);});
	sensor_Y_filter.setSubscriberFunction([](bool pinValue){t3Machine.input_sensorY(!pinValue);}); 	//NOTE: '!' because sensor is active low

	sensor_Z.setSubscriberFunction([](bool pinValue){sensor_Z_filter.input(pinValue);});
	sensor_Z_filter.setSubscriberFunction([](bool pinValue){t3Machine.input_sensorZ(!pinValue);}); 	//NOTE: '!' because sensor is active low

	button.setSubscriberFunction([](bool pinValue){button_filter.input(pinValue);});
	// button_filter.setSubscriberFunction([](bool pinValue){timingTest1.input(pinValue);});
	// button_filter.setSubscriberFunction([](bool pinValue){if (pinValue) {t3Machine.startMoving();}});
	button_filter.setSubscriberFunction([](bool pinValue){if (pinValue) {t3Machine.startHoming();}});

	vcom.setSubscriberFunction([](uint8_t data){vcom.transmit(data);});

	// button.setSubscriberFunction([](bool pinValue){button_filter.input(pinValue);});
	// // button.setSubscriberFunction([](bool pinValue){timingTest0.input(pinValue);});
	// button_filter.setSubscriberFunction([](bool pinValue){timingTest0.input(pinValue);});

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
