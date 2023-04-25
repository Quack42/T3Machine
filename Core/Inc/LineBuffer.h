#pragma once

#include "stringUtil.h"

#include <cstdint>
#include <functional>


template<unsigned int BufferSize>
class LineBuffer {
private:
	// Variables
	char buffer[BufferSize] = {};
	uint32_t writeIndex = 0;
	std::function<void(const char *, uint32_t)> subscriberFunction;
public:
	LineBuffer() :
			writeIndex(0)
	{
	}

	void setSubscriberFunction(std::function<void(const char *, uint32_t)> subscriberFunction) {
		this->subscriberFunction = subscriberFunction;
	}

	void input(char c) {
		const char kNewLineCharacters[3] = {'\r', '\n', '\0'};
		if (str_contains(kNewLineCharacters, c)) {
			// End of line found.
			if (writeIndex > 0) {
				// If we have content and a subscriber, advertise it to the subscriber.
				if (subscriberFunction) {
					subscriberFunction(buffer, writeIndex);
				}
			}

			// Reset buffer for the next line.
			writeIndex = 0;
		} else {
			// Not an end-of-line character; store it in the buffer.
			if (writeIndex < BufferSize) {
				// character still fits in the buffer.
				buffer[writeIndex] = c;
				writeIndex++;
			} else {
				//TODO: Line longer than can fit in buffer. Indicate failure.
				writeIndex = 0;
			}
		}
	}
};
