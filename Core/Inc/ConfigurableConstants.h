#pragma once

#include "TimeValue.h"

constexpr TimeValue DefaultTimeToWait = TimeValue(0, 5000, 0); 	//5000ms

constexpr float kButtonFuzzyFilterFlipTime = 3000.f;
constexpr float kSensorXFuzzyFilterFlipTime = 100.f;
constexpr float kSensorYFuzzyFilterFlipTime = 100.f;
constexpr float kSensorZFuzzyFilterFlipTime = 100.f;
