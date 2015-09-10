#include <FreeRTOS.h>
#include <task.h>
#include <system.h>

void WEAK _exit(int status) {
	(void) status;
#ifndef CONFIG_ASSERT_DISABLED
	CONFIG_ASSERT(0);
#else
	taskDISABLE_INTERRUPTS();
	for(;;);
#endif
}
