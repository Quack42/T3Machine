#pragma once

#include "PlatformSelection.h"
#include "PinIdentifier.h"

//outputs
extern PinIdentifier<Platform> ld3_pinIdentifier;
extern PinIdentifier<Platform> ld4_pinIdentifier;
extern PinIdentifier<Platform> ld5_pinIdentifier;
extern PinIdentifier<Platform> ld6_pinIdentifier;

extern PinIdentifier<Platform> stepPin_X_pinIdentifier;
extern PinIdentifier<Platform> stepPin_Y_pinIdentifier;
extern PinIdentifier<Platform> stepPin_Z_pinIdentifier;
extern PinIdentifier<Platform> directionPin_X_pinIdentifier;
extern PinIdentifier<Platform> directionPin_Y_pinIdentifier;
extern PinIdentifier<Platform> directionPin_Z_pinIdentifier;

//inputs
extern PinIdentifier<Platform> button_pinIdentifier;
extern PinIdentifier<Platform> sensor_X_pinIdentifier;
extern PinIdentifier<Platform> sensor_Y_pinIdentifier;
extern PinIdentifier<Platform> sensor_Z_pinIdentifier;
