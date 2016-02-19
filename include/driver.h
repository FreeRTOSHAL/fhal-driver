#ifndef DRIVER_H_
#ifdef LINKER_SCRIPT
#include <linker.h>
#define DEV(name, location) \
	SECTION_START(.rodata.dev.##name) \
	SYMBOL(_dev_##name); \
	KEEP(*(.rodata.dev.##name)) \
	SECTION_STOP(location)
#define DEV_DEFAULT(location) DEV(hal, location) \
	DEV(uart, location) \
	DEV(timer, location) \
	DEV(pwm, location) \
	DEV(capture, location) \
	DEV(spi, location) \
	DEV(accel, location) \
	DEV(gyro, location) \
	DEV(adc, location)
#endif
#endif
