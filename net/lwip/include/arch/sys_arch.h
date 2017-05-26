#ifndef SYS_ARCH_H_
#define SYS_ARCH_H_
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
typedef SemaphoreHandle_t sys_mutex_t;
typedef SemaphoreHandle_t sys_sem_t;
typedef QueueHandle_t sys_mbox_t;
typedef TaskHandle_t sys_thread_t;
typedef uint32_t sys_prot_t;
#include <stdlib.h>
#define LWIP_RAND() rand()
#endif
