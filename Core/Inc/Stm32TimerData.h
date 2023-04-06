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
		// Get maximum seconds per cycle
		const uint16_t kMaximumTicksPerCycle = 0xFFFF;
		float secondsPerCycle = kMaximumTicksPerCycle/getTicksPerSecond();
		// Convert to TimeValue
		return TimeValue (secondsPerCycle*MS_IN_A_S);
	}

	const TimeValue & getMaximumTimeToWait() const {
		return kMaximumTimeToWait;
	}
};
