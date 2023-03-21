// #ifdef STM32

#include "cpp_main.h"
#include "Pins.h"
#include "Devices.h"
#include "Stm32PinIdentifier.h"

extern TIM_HandleTypeDef htim10; 	//Used for timingManager


//pin interrupt service routine
void CPP_HAL_GPIO_EXTI_Callback(uint16_t pin) {
	if (pin == sensor_X.getPinIdentifier().getPin()) {
		sensor_X.isr();
		

		// steppingTask.stop();
		//for fun; keep LD5 (red) LED matched to sensor_X value
		// ld5.setValue(sensor_X.getValue());
		// timingManager.wakeup(); 	//TODO: get this to work
	}
	if (pin == sensor_Y.getPinIdentifier().getPin()) {
		sensor_Y.isr();
		//for fun; keep LD3 (orange) LED matched to sensor_Y value
		// ld3.setValue(sensor_Y.getValue());
		// timingManager.wakeup(); 	//TODO: get this to work
	}
	if (pin == sensor_Z.getPinIdentifier().getPin()) {
		sensor_Z.isr();
		//for fun; keep LD6 (color: blue?) LED matched to sensor_Z value
		// ld6.setValue(sensor_Z.getValue());
		// timingManager.wakeup(); 	//TODO: get this to work
	}

	if (pin == button.getPinIdentifier().getPin()) {
	// if (pin == button.getPinIdentifier().getPin() && !button.getValue()) {
		// steppingTask.startSteppingTask(1000);
		// t3Machine.startHoming();
		// timingManager._speedUp();
		button.isr();
	}
}

//timer interrupt service routine
void CPP_HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim) {
	if(htim == &htim10) {
		// timingManager.awake();
		timingManager.timerISR();
		// ld6.toggle();
	}
}

// #endif //STM32
