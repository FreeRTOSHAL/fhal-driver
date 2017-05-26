#include <arch/sys_arch.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <lwip/err.h>
#include <lwip/sys.h>

SemaphoreHandle_t mutex;

void sys_init(void) {
	mutex = xSemaphoreCreateRecursiveMutex();
	CONFIG_ASSERT(mutex != NULL);
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count) {
	*sem = xSemaphoreCreateCounting(count, 0);
	if (*sem == NULL) {
		return ERR_MEM;
	}
	return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem) {
	vSemaphoreDelete(*sem);
}
void sys_sem_signal(sys_sem_t *sem) {
	xSemaphoreGive(*sem);
}
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout) {
	TickType_t starttime = xTaskGetTickCount();
	TickType_t endtime;
	TickType_t waitime = (timeout == 0) ? portMAX_DELAY : (timeout / portTICK_PERIOD_MS);
	BaseType_t ret;
	ret = xSemaphoreTake(*sem, waitime);
	if (ret != pdTRUE) {
		return SYS_ARCH_TIMEOUT;
	}
	endtime = xTaskGetTickCount();
	if (endtime < starttime) {
		return ((portMAX_DELAY - starttime) + endtime) * portTICK_PERIOD_MS;
	} else {
		return (endtime - starttime) * portTICK_PERIOD_MS;
	}
}
int sys_sem_valid(sys_sem_t *sem) {
	return *sem != NULL;
}
void sys_sem_set_invalid(sys_sem_t *sem) {
	*sem = NULL;
}

err_t sys_mutex_new(sys_mutex_t *mutex) {
	*mutex = xSemaphoreCreateRecursiveMutex();
	if (*mutex == NULL) {
		return ERR_MEM;
	}
	return ERR_OK;
}

void sys_mutex_lock(sys_mutex_t *mutex) {
	xSemaphoreTakeRecursive(*mutex, portMAX_DELAY);
}
void sys_mutex_unlock(sys_mutex_t *mutex) {
	xSemaphoreGiveRecursive(*mutex);
}
void sys_mutex_free(sys_mutex_t *mutex) {
	vSemaphoreDelete(*mutex);
}
int sys_mutex_valid(sys_mutex_t *mutex) {
	return *mutex != NULL;
}
void sys_mutex_set_invalid(sys_mutex_t *mutex) {
	*mutex = NULL;
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size) {
	*mbox = xQueueCreate(size, sizeof(void *));
	if (*mbox == NULL) {

		return ERR_MEM;
	}
	return ERR_OK;
}
void sys_mbox_free(sys_mbox_t *mbox) {
	vQueueDelete(mbox);
}
void sys_mbox_post(sys_mbox_t *mbox, void *msg) {
	xQueueSendToBack(*mbox, &msg, portMAX_DELAY);
}
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg) {
	BaseType_t ret = xQueueSendToBack(*mbox, &msg, 0);
	if (ret != pdTRUE) {
		return ERR_MEM;
	}
	return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout) {
	TickType_t starttime = xTaskGetTickCount();
	TickType_t endtime;
	TickType_t waitime = (timeout == 0) ? portMAX_DELAY : (timeout / portTICK_PERIOD_MS);
	BaseType_t ret;
	ret = xQueueReceive(*mbox, msg, waitime);
	if (ret != pdTRUE) {
		return SYS_ARCH_TIMEOUT;
	}
	endtime = xTaskGetTickCount();
	if (endtime < starttime) {
		return ((portMAX_DELAY - starttime) + endtime) * portTICK_PERIOD_MS;
	} else {
		return (endtime - starttime) * portTICK_PERIOD_MS;
	}
}
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg) {
	BaseType_t ret;
	ret = xQueueReceive(*mbox, msg, 0);
	if (ret != pdTRUE) {
		return SYS_MBOX_EMPTY;
	}
	return 0;
}
int sys_mbox_valid(sys_mbox_t *mbox) {
	return *mbox != NULL;
}
void sys_mbox_set_invalid(sys_mbox_t *mbox) {
	*mbox = NULL;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio) {
	sys_thread_t task;
	BaseType_t ret;
	ret = xTaskCreate(thread, name, (stacksize >> 2), arg, prio, &task);
	CONFIG_ASSERT(ret == pdTRUE);
	return task;
}
sys_prot_t sys_arch_protect(void) {
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	return 0;
}
void sys_arch_unprotect(sys_prot_t pval) {
	(void) pval;
	xSemaphoreGiveRecursive(mutex);
}
u32_t sys_now(void) {
	TickType_t time = xTaskGetTickCount();
	return time * portTICK_PERIOD_MS;
}
