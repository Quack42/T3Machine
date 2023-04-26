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

#include "GCodeInterpreter.h"
#include "LineBuffer.h"

// #include "TimingTest0.h"
// #include "TimingTest1.h"


// TimingTest0<Platform> timingTest0(processManager, timingManager, ld3, ld6);
// TimingTest1<Platform> timingTest1(ld3, ld6, steppingTaskTimer);
LineBuffer<100> lineBuffer;
GCodeInterpreter<Platform, T3Machine<Platform,M415C<Platform>,M415C<Platform>,DRV8825_MovementControl<Platform>> > gCodeInterpreter(processManager, t3Machine);



void cpp_main(void) {
	// Initialize platform.
	timingManager.init();

	// Initialize pins.
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

	// Initialize filters.
	button_filter.init(button.getValue());
	sensor_X_filter.init(sensor_X.getValue());
	sensor_Y_filter.init(sensor_Y.getValue());
	sensor_Z_filter.init(sensor_Z.getValue());
	// sensor_X_filter.init(sensor_X.getValue());

	// Initialize devices.
	m415c_X.init();
	m415c_Y.init();
	drv8825_Z.init();

	t3Machine.init();

	// Initialize data channels.
	gCodeInterpreter.init();

	// Initialize timers.
	steppingTaskXTimer.init();
	steppingTaskYTimer.init();
	steppingTaskZTimer.init();

	// Initialize tests.
	// timingTest1.init();

	////////////////////////
	// Setup data connections.

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

	vcom.setSubscriberFunction([](uint8_t data){lineBuffer.input(data);});
	lineBuffer.setSubscriberFunction([](const char * line, uint32_t lineLength){
			// vcom.transmit(reinterpret_cast<const uint8_t *>(line), lineLength);
			// vcom.transmit(reinterpret_cast<const uint8_t *>("\n"), 1);
			gCodeInterpreter.input(line, lineLength);
	});
	// lineBuffer.setSubscriberFunction([](const char * line, uint32_t lineLength){vcom.transmit(reinterpret_cast<const uint8_t *>(line), lineLength);});
	gCodeInterpreter.setChannelToHost([](const char * msg, uint32_t length) {vcom.transmit(reinterpret_cast<const uint8_t *>(msg), length);});
	// vcom.setSubscriberFunction([](uint8_t data){vcom.transmit(data);});


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
