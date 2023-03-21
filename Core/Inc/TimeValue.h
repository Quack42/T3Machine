#pragma once

#include <cstdint>


#define MS_IN_A_DAY (24*60*60*1000)
#define US_IN_A_MS (1000)
#define MS_IN_A_S (1000)
#define US_IN_A_S (US_IN_A_MS * MS_IN_A_S)


class TimeValue {
public:
	uint32_t days = 0;
	uint32_t ms = 0;
	uint16_t us = 0;

	constexpr TimeValue() :
			days(0),
			ms(0),
			us(0)
	{

	}

	constexpr TimeValue(uint16_t days, uint32_t ms, uint16_t us) :
			days(days),
			ms(ms),
			us(us)
	{

	}

	bool operator>(const TimeValue & rhs) const {
		if (days == rhs.days) {
			if (ms == rhs.ms) {
				return us > rhs.us;
			}
			//else
			return ms > rhs.ms;
		}
		//else
		return days > rhs.days;
	}

	TimeValue operator-(const TimeValue & rhs) const {
		TimeValue ret(days, ms, us);
		uint32_t msToSubtract = rhs.ms;
		uint32_t daysToSubtract = rhs.days;
		if (ret.us < rhs.us) {
			msToSubtract++;
			ret.us = ret.us + 1000 - rhs.us;
		} else {
			ret.us -= rhs.us;
		}

		if (ret.ms < msToSubtract) {
			daysToSubtract++;
			ret.ms = ret.ms + MS_IN_A_DAY - rhs.ms; 
		} else {
			ret.ms -= msToSubtract;
		}

		if (ret.days < daysToSubtract) {
			//NOTE: because of the UNSIGNED nature of the numbers, net negatives will be interpreted as 0
			ret.us = 0;
			ret.ms = 0;
			ret.days = 0;
		} else {
			ret.days -= daysToSubtract;
		}
		return ret;
	}

	TimeValue & operator=(const TimeValue & rhs) {
		this->us = rhs.us;
		this->ms = rhs.ms;
		this->days = rhs.days;
		return *this;
	}

	TimeValue & operator-=(const TimeValue & rhs) {
		*this = (*this - rhs);
		return *this;
	}
};
