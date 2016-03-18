#ifndef DRIVER_H_
#ifdef LINKER_SCRIPT
/**
 * \defgroup DRIVER Driver specific Linker Macros 
 * \ingroup LINKER
 * \code
 * #include <driver.h>
 * \endcode
 * \{
 */
#include <linker.h>
/**
 * Define Device Array Sections
 * \param name Driver Name like uart, timer, pwm, ...
 * \param location Location
 */
#define DEV(name, location) \
	SECTION_START(.rodata.dev.##name) \
	SYMBOL(_dev_##name); \
	KEEP(*(.rodata.dev.##name)) \
	SYMBOL(_dev_##name##_end); \
	SECTION_STOP(location)
/**
 * Default Devices
 * \param location Location
 */
#define DEV_DEFAULT(location) DEV(hal, location) \
	DEV(gpio, location) \
	DEV(uart, location) \
	DEV(timer, location) \
	DEV(pwm, location) \
	DEV(capture, location) \
	DEV(spi, location) \
	DEV(accel, location) \
	DEV(gyro, location) \
	DEV(adc, location) \
	DEV(example, location) 
#endif
/**\}*/
#endif
