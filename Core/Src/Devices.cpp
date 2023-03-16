#include "Devices.h"

//platform
ProcessManager<Platform> processManager;

//output pins
PinData<Platform> ld3(ld3_pinIdentifier);
PinData<Platform> ld4(ld4_pinIdentifier);
PinData<Platform> ld5(ld5_pinIdentifier);
PinData<Platform> ld6(ld6_pinIdentifier);

PinData<Platform> stepPin_X(stepPin_X_pinIdentifier);
PinData<Platform> stepPin_Y(stepPin_Y_pinIdentifier);
PinData<Platform> stepPin_Z(stepPin_Z_pinIdentifier);
PinData<Platform> directionPin_X(directionPin_X_pinIdentifier);
PinData<Platform> directionPin_Y(directionPin_Y_pinIdentifier);
PinData<Platform> directionPin_Z(directionPin_Z_pinIdentifier);

//interrupt pins
ExternalInterruptPin<Platform> button(processManager, button_pinIdentifier, false);
ExternalInterruptPin<Platform> sensor_X(processManager, sensor_X_pinIdentifier, true);
ExternalInterruptPin<Platform> sensor_Y(processManager, sensor_Y_pinIdentifier, true);
ExternalInterruptPin<Platform> sensor_Z(processManager, sensor_Z_pinIdentifier, true);

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
