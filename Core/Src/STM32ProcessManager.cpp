#include "ProcessManager.h"
#include "Stm32F407Platform.h"
#include "stm32f4xx_hal.h"

//#ifdef STM32
template<>
void ProcessManager<Stm32F407Platform>::disableInterrupts() {
	__disable_irq();
}

template<>
void ProcessManager<Stm32F407Platform>::enableInterrupts() {
	__enable_irq();
}
//#endif //STM32
