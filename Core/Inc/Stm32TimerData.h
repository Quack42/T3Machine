#pragma once

#include "TimerData.h"
#include "Stm32F407Platform.h"
#include "TimeValue.h"

#include "stm32f4xx_hal.h"


#include <cstdint>

template<>
class TimerData<Stm32F407Platform> {
private:
	TIM_HandleTypeDef & timerHandle;

public:
	constexpr TimerData(TIM_HandleTypeDef & timerHandle) :
			timerHandle(timerHandle)
	{

	}

	TIM_HandleTypeDef & getTimerHandle() const {
		return timerHandle;
	}
};

template<>
class TimerConstants<Stm32F407Platform> {
private:
	const unsigned long kTimerBase;
	const uint32_t kClockFrequency;
	const uint32_t kPrescaler;
	const uint32_t kClockDivisionRegisterValue;
	const TimeValue kMaximumTimeToWait;

public:
	constexpr TimerConstants(const unsigned long & kTimerBase, const uint32_t & kClockFrequency, const uint32_t & kPrescaler, const uint32_t & kClockDivisionRegisterValue) :
			kTimerBase(kTimerBase),
			kClockFrequency(kClockFrequency),
			kPrescaler(kPrescaler),
			kClockDivisionRegisterValue(kClockDivisionRegisterValue),
			kMaximumTimeToWait(calcMaximumTimeToWait())
	{

	}

	TIM_TypeDef * getTimerBase() const {
		return reinterpret_cast<TIM_TypeDef*>(kTimerBase);
	}

	constexpr const uint32_t & getClockFrequency() const {
		return kClockFrequency;
	}

	constexpr const uint32_t & getPrescaler() const {
		return kPrescaler;
	}

	constexpr const uint32_t & getClockDivisionRegisterValue() const {
		return kClockDivisionRegisterValue;
	}

	constexpr const uint32_t getClockDivision() const {
		switch (getClockDivisionRegisterValue()) {
			default:
			case TIM_CLOCKDIVISION_DIV1:
				return 1;
			case TIM_CLOCKDIVISION_DIV2:
				return 2;
			case TIM_CLOCKDIVISION_DIV4:
				return 4;
		}
	}

	constexpr float getTicksPerSecond() const {
		float ticksPerSecond = static_cast<float>(getClockFrequency()) / (getClockDivision() * getPrescaler());
		return ticksPerSecond;
	}

	constexpr TimeValue calcMaximumTimeToWait() const {
		// //get seconds per tick
		// float secondsPerTick = 1.0f/getTicksPerSecond();

		// //get maximum ticks per cycle
		// uint16_t kMaximumTicksPerCycle = 0xFFFF;

		// float maximumTimeInS = kMaximumTicksPerCycle * secondsPerTick;
		// float maximumTimeInMS = maximumTimeInS / MS_IN_A_S;
		// return TimeValue (maximumTimeInMS);
		return TimeValue (0,5000,0);
	}

	// const TimeValue getMaximumTimeToWait() const {
	const TimeValue & getMaximumTimeToWait() const {
		// return TimeValue(0,1000,0); 	//TODO: remove this
		return kMaximumTimeToWait; 	//TODO: uncomment this
	}
};
