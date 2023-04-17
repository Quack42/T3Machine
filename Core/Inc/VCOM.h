#pragma once

// For STM32F407 DISC:  https://medium.com/@pasindusandima/stm32-usb-virtual-com-port-vcp-bc7cb1bd5f5

#include "ProcessManager.h"
#include "CyclicBuffer.h"
#include "ProcessRequest.h"

#include <functional>
#include <cstdint>

template<typename Platform>
class VCOMTXInterface {
public:
	uint32_t _startTransmission(uint8_t * dataToTransmit, uint32_t lengthOfDataToTransmit);
};


template<typename Platform, int TXBufferSize, int RXBufferSize>
class VCOM_Buffered {
private:
	// References
	ProcessManager<Platform> & processManager;

	// Components
	CyclicBuffer<uint8_t, TXBufferSize> txBuffer;
	CyclicBuffer<uint8_t, RXBufferSize> rxBuffer;

	std::function<void(uint8_t)> rxCallback;

	VCOMTXInterface<Platform> vcomTXInterface;

	ProcessRequest conveyRXDataProcess;
	ProcessRequest transmitTXDataProcess;

	// Variables
	unsigned int transmittingDataLength;

public:
	VCOM_Buffered(ProcessManager<Platform> & processManager) :
			// References
			processManager(processManager),
			// Components
			conveyRXDataProcess(std::bind(&VCOM_Buffered<Platform, TXBufferSize, RXBufferSize>::advertise, this)),
			transmitTXDataProcess(std::bind(&VCOM_Buffered<Platform, TXBufferSize, RXBufferSize>::transmissionLogic, this)),
			// Variables
			transmittingDataLength(0)
	{
	}

	void setSubscriberFunction(std::function<void(uint8_t)> rxCallback) {
		this->rxCallback = rxCallback;
	}

	// Transmit data via Virtual COM ('UART' via USB).
	unsigned int transmit (uint8_t byte) {
		return transmit(&byte, 1);
	}

	unsigned int transmit(const uint8_t * data, unsigned int dataLength) {
		unsigned int ret=0;
		if (txBuffer.computeFreeSpace() < dataLength) {
			// Cannot buffer all data.. 
		} else {
			// Buffer all data
			txBuffer.write(data, dataLength);
		}

		// Trigger transmission process.
		processManager.requestProcess(transmitTXDataProcess);

		// Return how much data has been buffered to transmit.
		return ret;
	}

	uint8_t read() {
		return rxBuffer.read();
	}

	void transmissionCompleteISR() {
		txBuffer.markAsRead(transmittingDataLength);
		transmittingDataLength = 0;

		if (!txBuffer.isEmpty()) {
			processManager.requestProcess(transmitTXDataProcess);
		}
	}

	void receivedISR(uint8_t * data, uint32_t dataLength) {
		if (rxBuffer.computeFreeSpace() >= dataLength) {
			rxBuffer.write(data, dataLength);
		} else {
			//not enough free space
		}
		processManager.requestProcess(conveyRXDataProcess);
	}

private:

	void advertise() {
		if (rxCallback) {
			while (!rxBuffer.isEmpty()){
				rxCallback(rxBuffer.read());
			}
		}
	}

	void transmissionLogic() {
		// Get data to transmit.
		uint8_t * dataToTransmit = txBuffer.getReadingPointerToRawData();
		// Get the length of the data to transmit.
		uint32_t lengthOfDataToTransmit = txBuffer.getConsecutiveLength();
		// Queue it for transmission via platform specific function.
		uint32_t queuedBytesForTransmission = vcomTXInterface._startTransmission(dataToTransmit, lengthOfDataToTransmit);
		// Remember how much data we've queued. We can only clear it after transmission has finished (the transmission process might use the provided buffer).
		transmittingDataLength = queuedBytesForTransmission;
	}

};
