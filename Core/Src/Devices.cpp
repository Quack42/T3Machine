#include "Devices.h"

#include "FuzzyFilter.h"

//platform
ProcessManager<Platform> processManager;
TimingManager<Platform> timingManager(processManager, timingManagerTimer);


//output pins
OutputPin<Platform> ld3(ld3_pinIdentifier);
OutputPin<Platform> ld4(ld4_pinIdentifier);
OutputPin<Platform> ld5(ld5_pinIdentifier);
OutputPin<Platform> ld6(ld6_pinIdentifier);

OutputPin<Platform> stepPin_X(stepPin_X_pinIdentifier);
OutputPin<Platform> stepPin_Y(stepPin_Y_pinIdentifier);
OutputPin<Platform> stepPin_Z(stepPin_Z_pinIdentifier);
OutputPin<Platform> directionPin_X(directionPin_X_pinIdentifier);
OutputPin<Platform> directionPin_Y(directionPin_Y_pinIdentifier);
OutputPin<Platform> directionPin_Z(directionPin_Z_pinIdentifier);

//interrupt pins
ExternalInterruptPin<Platform> button(processManager, button_pinIdentifier);
ExternalInterruptPin<Platform> sensor_X(processManager, sensor_X_pinIdentifier); 	//Active Low
ExternalInterruptPin<Platform> sensor_Y(processManager, sensor_Y_pinIdentifier); 	//Active Low
ExternalInterruptPin<Platform> sensor_Z(processManager, sensor_Z_pinIdentifier); 	//Active Low

//filters
FuzzyFilter<Platform> button_filter(processManager, timingManager, 1000.f);
FuzzyFilter<Platform> sensor_X_filter(processManager, timingManager);

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
