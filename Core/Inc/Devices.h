#pragma once

#include "PlatformSelection.h"
#include "Pins.h"

//process control
#include "Timers.h"
#include "TimingManager.h"
#include "ProcessManager.h"

//entities
#include "MyGPIO.h"
#include "MyExternalInterruptPin.h"
#include "FuzzyFilter.h"
#include "VCOM.h"
#include "M415C.h"
#include "DRV8825.h"
#include "T3Machine.h"

//platform
extern ProcessManager<Platform> processManager;
extern TimingManager<Platform> timingManager;

//Output pins
extern OutputPin<Platform> ld3; 	//orange
extern OutputPin<Platform> ld4; 	//green
extern OutputPin<Platform> ld5; 	//red
extern OutputPin<Platform> ld6; 	//blue

extern OutputPin<Platform> stepPin_X;
extern OutputPin<Platform> stepPin_Y;
extern OutputPin<Platform> stepPin_Z;
extern OutputPin<Platform> directionPin_X;
extern OutputPin<Platform> directionPin_Y;
extern OutputPin<Platform> directionPin_Z;

//interrupt pins
extern ExternalInterruptPin<Platform> button;
extern ExternalInterruptPin<Platform> sensor_X;
extern ExternalInterruptPin<Platform> sensor_Y;
extern ExternalInterruptPin<Platform> sensor_Z;

//filters
extern FuzzyFilter<Platform> button_filter;
extern FuzzyFilter<Platform> sensor_X_filter;
extern FuzzyFilter<Platform> sensor_Y_filter;
extern FuzzyFilter<Platform> sensor_Z_filter;

//components
extern VCOM_Buffered<Platform, kVCOM_TXBufferSize, kVCOM_RXBufferSize> vcom;



//devices
extern M415C<Platform> m415c_X;
extern M415C<Platform> m415c_Y;
extern DRV8825_MovementControl<Platform> drv8825_Z;
extern T3Machine<
	Platform,
	M415C<Platform>,
	M415C<Platform>,
	DRV8825_MovementControl<Platform>
	> t3Machine;
