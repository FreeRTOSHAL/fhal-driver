#ifndef CC_H_
#define CC_H_
#include <FreeRTOS.h>
#include <sys/time.h>
#define LWIP_PLATFORM_ASSERT(message) do {printf(message); CONFIG_ASSERT(0);} while (0)
#endif
