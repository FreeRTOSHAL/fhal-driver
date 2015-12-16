#ifndef DRIVER_H_
#ifdef LINKER_SCRIPT
#include <linker.h>
#define DEV(name, location) \
	SECTION_START(.rodata.dev.##name) \
	SYMBOL(_dev_##name); \
	KEEP(*(.rodata.dev.##name)) \
	SECTION_STOP(location)
#endif
#endif
