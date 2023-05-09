#pragma once

#include "TimeValue.h"

#include <cstdint>

constexpr TimeValue DefaultTimeToWait = TimeValue(0, 5000, 0); 	//5000ms

constexpr float kButtonFuzzyFilterFlipTime = 100.f/0.7f;
constexpr float kSensorXFuzzyFilterFlipTime = 100.f;
constexpr float kSensorYFuzzyFilterFlipTime = 100.f;
constexpr float kSensorZFuzzyFilterFlipTime = 100.f;

//system specific constants
constexpr uint32_t kSystemFrequency = 168000000ul;
constexpr uint32_t kAPB1TimerFrequency = kSystemFrequency/2;
constexpr uint32_t kAPB2TimerFrequency = kSystemFrequency;

constexpr unsigned long kTimingManagerTicksPerSecond = 10000ul;
constexpr unsigned long kTimingManagerTicksPerMS = kTimingManagerTicksPerSecond / MS_IN_A_S;
constexpr unsigned long kTimingManagerUSPerTick = (1.f/static_cast<float>(kTimingManagerTicksPerSecond) * US_IN_A_S) + 0.5f; 	//+0.5f for rounding


constexpr unsigned long kSteppingTaskTicksPerSecond = 10000ul;
// constexpr unsigned long kSteppingTaskTicksPerMS = kSteppingTaskTicksPerSecond / MS_IN_A_S;
// constexpr unsigned long kSteppingTaskUSPerTick = (1.f/static_cast<float>(kSteppingTaskTicksPerSecond) * US_IN_A_S) + 0.5f; 	//+0.5f for rounding

// Constants for VCOM
constexpr unsigned int kVCOM_TXBufferSize = 400;
constexpr unsigned int kVCOM_RXBufferSize = 400;

// Constants for mathematical model of machine.
constexpr float kMMPerRotationXAxis = 2.0f; 	//TODO: find this
constexpr float kMMPerRotationYAxis = 2.0f; 	//TODO: find this
constexpr float kMMPerRotationZAxis = 8.0f; 	//TODO: find this

constexpr unsigned int kTicksPerRotationXAxis = 200;
constexpr unsigned int kTicksPerRotationYAxis = 200;
constexpr unsigned int kTicksPerRotationZAxis = 200; 	//TODO: find this

constexpr float kMMPerTickXAxis = kMMPerRotationXAxis / kTicksPerRotationXAxis;
constexpr float kMMPerTickYAxis = kMMPerRotationYAxis / kTicksPerRotationYAxis;
constexpr float kMMPerTickZAxis = kMMPerRotationZAxis / kTicksPerRotationZAxis;

constexpr float kMovementRangeX = 10.f; 	// In mm. 	//TODO: find this
constexpr float kMovementRangeY = 10.f; 	// In mm. 	//TODO: find this
constexpr float kMovementRangeZ = 10.f; 	// In mm. 	//TODO: find this

// GCodeInterpreter
constexpr unsigned int kMaximumNumberOfParameters = 10;
constexpr bool kDebugMode = true;