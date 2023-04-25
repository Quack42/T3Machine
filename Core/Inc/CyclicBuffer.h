#pragma once

#include <algorithm>

// This cyclic buffer is not thread-safe to use. This responsibility lies with the user.

template<typename Type, int BufferSize>
class CyclicBuffer {
private:
	Type buffer[BufferSize];
	unsigned int iWrite;
	unsigned int iRead;
public:
	CyclicBuffer() :
			iWrite(0),
			iRead(0)
	{
	}

	bool hasSpace() const {
		// The buffer is full if the increment of the writing index equals the reading index.
		return (computeIncrement(iWrite) != iRead);
	}

	bool isFull() const {
		return !hasSpace();
	}

	bool isEmpty() const {
		// Buffer is empty if reading index equals the writing index.
		return (iRead == iWrite);
	}

	void write(Type t) {
		// Write data into buffer.
		buffer[iWrite] = t;
		// Increment writing index.
		iWrite = computeIncrement(iWrite);
	}

	void write(const Type * data, unsigned int length) {
		// Write all data into buffer
		for (unsigned int i=0; i < length; i++) {
			write(data[i]);
		}
	}

	Type peek() {
		// Copy reference to data in buffer.
		Type & ret = buffer[iRead];
		// Return referenced data.
		return ret;
	}

	Type read() {
		// Copy reference to data in buffer.
		Type & ret = buffer[iRead];
		// Increment reading index.
		iRead = computeIncrement(iRead);
		// Return referenced data.
		return ret;
	}

	void markAsRead(unsigned int length) {
		// Make sure iRead doesn't pass iWrite; given proper use this is not needed, so just directly copy 'length' while this is in private use.
		//unsigned int lengthToMarkAsRead = std::min(computeUnreadLength(), length);
		unsigned int lengthToMarkAsRead = length;

		// Increment reading index.
		iRead = computeIncrement(iRead, lengthToMarkAsRead);
	}

	unsigned int getConsecutiveLength() {
		// This function asks for the consecutive data that is unread.
		// This data starts at the reading index and ends at either (whichever is first) the end of the raw buffer or the writing index
		// Compute the length of unread data.
		unsigned int unreadLength = computeUnreadLength();
		// Compute the distance between the reading index and the end of the raw buffer.
		unsigned int lengthTillEndOfRawBuffer = (BufferSize - iRead); 	//This assumes that the reading index is always of a lower value than the BufferSize (which it should be).
		// Return the lowest value
		return std::min(unreadLength, lengthTillEndOfRawBuffer);
	}

	Type * getReadingPointerToRawData() {
		return &buffer[iRead];
	}

	unsigned int computeUnreadLength() {
		unsigned int ret=0;
		if (iWrite < iRead) {
			ret = iWrite + BufferSize - iRead;
		} else {
			ret = iWrite - iRead;
		}
		return ret;
	}

	unsigned int computeFreeSpace() {
		return (BufferSize - computeUnreadLength());
	}

private:
	unsigned int computeIncrement(unsigned int index, unsigned int increment=1) const {
		// Increment
		index += increment;
		// Loop if necessary
		index %= BufferSize;
		// Return
		return index;
	}
};
