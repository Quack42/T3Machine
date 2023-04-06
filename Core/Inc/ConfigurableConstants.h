#pragma once

#include "TimeValue.h"

constexpr TimeValue DefaultTimeToWait = TimeValue(0, 5000, 0); 	//5000ms

constexpr float kButtonFuzzyFilterFlipTime = 100.f/0.7f;
constexpr float kSensorXFuzzyFilterFlipTime = 100.f;
constexpr float kSensorYFuzzyFilterFlipTime = 100.f;
constexpr float kSensorZFuzzyFilterFlipTime = 100.f;

//system specific constants
constexpr uint32_t kSystemFrequency = 168000000ul;

constexpr unsigned long kTimingManagerTicksPerSecond = 10000ul;
constexpr unsigned long kTimingManagerTicksPerMS = kTimingManagerTicksPerSecond / MS_IN_A_S;
constexpr unsigned long kTimingManagerUSPerTick = (1.f/static_cast<float>(kTimingManagerTicksPerSecond) * US_IN_A_S) + 0.5f; 	//+0.5f for rounding


constexpr unsigned long kSteppingTaskTicksPerSecond = 10000ul;
// constexpr unsigned long kSteppingTaskTicksPerMS = kSteppingTaskTicksPerSecond / MS_IN_A_S;
// constexpr unsigned long kSteppingTaskUSPerTick = (1.f/static_cast<float>(kSteppingTaskTicksPerSecond) * US_IN_A_S) + 0.5f; 	//+0.5f for rounding
