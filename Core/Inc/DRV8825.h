#pragma once

//#include "IOPin.h"
#include "PinControl.h"

//NOTE: The current limiter is done by hand. See: https://lastminuteengineers.com/drv8825-stepper-motor-driver-arduino-tutorial/

template<typename Platform>
class DRV8825_MovementControl {
private:
	PinData<Platform> & stepPin;
	PinData<Platform> & directionPin;

public:
	DRV8825_MovementControl(PinData<Platform> & stepPin,
			PinData<Platform> & directionPin) :
		stepPin(stepPin),
		directionPin(directionPin)
	{

	}

	void init() {
		//..
	}

	void setStepPin(const bool & pinValue) {
		stepPin.setValue(pinValue); 	//TODO: figure out if the step takes place on low->high or high->low
	}

	void setDirectionPin(const bool & pinValue) {
		directionPin.setValue(pinValue);
	}
};


/*
template<typename Platform>
class DRV8825 {
private:
	DRV8825_MovementControl<Platform> movementControl;
	PinControlOut<ActiveLow> pc_enable;
	PinData<Platform> & pin_microstep0;
	PinData<Platform> & pin_microstep1;
	PinData<Platform> & pin_microstep2;
	PinControlOut<ActiveLow> pc_reset;
	PinControlOut<ActiveLow> pc_sleep;
	// PinData<Platform> & pin_step;
	// PinData<Platform> & pin_dir;
	// PinControlIn<ActiveLow> pc_fault;
public:
	DRV8825_FULL(PinData<Platform> & pin_enable,
			PinData<Platform> & pin_microstep0,
			PinData<Platform> & pin_microstep1,
			PinData<Platform> & pin_microstep2,
			PinData<Platform> & pin_reset,
			PinData<Platform> & pin_sleep,
			PinData<Platform> & pin_step,
			PinData<Platform> & pin_dir//,
			// IOPin & pin_fault
	) : 
			movementControl(pin_step, pin_dir),
			pc_enable(pin_enable),
			pin_microstep0(pin_microstep0),
			pin_microstep1(pin_microstep1),
			pin_microstep2(pin_microstep2),
			pc_reset(pin_reset),
			pc_sleep(pin_sleep),
			// pin_step(pin_step),
			// pin_dir(pin_dir)//,
			// pc_fault(pin_fault),
	{
		sleepState();
	}

	void init() {
		moveState();
		movementControl.init();
	}

	void setStepPin(const bool & pinValue) {
		movementControl.setStepPin(pinValue);
	}

	void setDirectionPin(const bool & pinValue) {
		movementControl.setDirectionPin(pinValue);
	}

	void resetState() {
		pc_enable.deactivate();
		pc_sleep.deactivate();
		pc_reset.activate();
		//TODO: wait for a few ms.
		pc_reset.deactivate();
		//TODO: wait for a few ms. ??
	}

	void sleepState() {
		pc_sleep.activate();
		pc_enable.deactivate();
		pc_reset.deactivate();
	}

	void moveState() {
		pc_reset.deactivate();
		pc_sleep.deactivate();
		pc_enable.activate();
	}


	// void step() {
	// 	moveState();

	// 	pin_step.high();
	// 	//TODO: wait for a few ms.
	// 	pin_step.low();
	// 	//TODO: wait for a few ms. ??
	// }

	// void clockwise() {
	// 	pin_dir.high();
	// }
	// void counterClockwise() {
	// 	pin_dir.low();
	// }

	enum MicrostepResolution {
		_1_1,
		_1_2,
		_1_4,
		_1_8,
		_1_16,
		_1_32
	};

	void setMicrostepResolution(const MicrostepResolution & resolution) {
		switch(resolution) {
			case _1_1:
				pin_microstep0.low();
				pin_microstep1.low();
				pin_microstep2.low();
				break;
			case _1_2:
				pin_microstep0.high();
				pin_microstep1.low();
				pin_microstep2.low();
				break;
			case _1_4:
				pin_microstep0.low();
				pin_microstep1.high();
				pin_microstep2.low();
				break;
			case _1_8:
				pin_microstep0.high();
				pin_microstep1.high();
				pin_microstep2.low();
				break;
			case _1_16:
				pin_microstep0.low();
				pin_microstep1.low();
				pin_microstep2.high();
				break;
			case _1_32:
				pin_microstep0.high();
				pin_microstep1.low();
				pin_microstep2.high();
				break;
		}
	}

	// bool isFaulty() {
		// return pc_fault.isActive();
	// }
};
*/
