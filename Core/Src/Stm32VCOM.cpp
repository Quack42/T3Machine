#include "VCOM.h"
#include "Stm32F407Platform.h"

#include "usbd_cdc_if.h"

template<>
uint32_t VCOMTXInterface<Stm32F407Platform>::_startTransmission(uint8_t * dataToTransmit, uint32_t lengthOfDataToTransmit) {
	// A little parameter sanity checking: make sure at least something is intended to be sent.
	if (lengthOfDataToTransmit > 0) {
		// If there's something to transmit: queue it.
		uint8_t resultCode = CDC_Transmit_FS(dataToTransmit, lengthOfDataToTransmit);
		// Check the result.
		if (resultCode == USBD_OK) {
			// If the result is OK, indicate 'lengthOfDataToTransmit' amount of bytes are in the process of transmission.
			return lengthOfDataToTransmit;
		}
	}

	// If the process failed for any reason, indicate 0 bytes are in the process of transmission.
	return 0;
}
