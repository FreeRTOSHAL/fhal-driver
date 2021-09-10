/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef LINKER_H_
/**
 * \defgroup LINKER Linker Script Macros
 * \ingroup HAL
 * \code
 * #include <linker.h>
 * \endcode
 * This Marcos is for creating Linker Scripts
 * 
 * The Linker Files are create by C Preprocessor. 
 * 
 * Example Code m0.lds.S: 
 * \code
 * #include <linker.h>
 * #include <driver.h>
 *
 * ENTRY(reset_handler)
 * 
 * MEM_START
 * 	MEM_ADD(flash, 0x00000000, 0x20000)
 * 	MEM_ADD(sram, 0x00020000, 0x20000)
 * MEM_STOP
 * 
 * SECTIONS_START
 * 	. = ORIGIN(flash);
 * 	VECTOR_START
 * 		VECTOR_DEFAULT
 * 	VECTOR_STOP(flash)
 * 
 * 	TEXT_START
 * 		DATA_TABLE_START
 * 			DATA_TABLE(.data)
 * 		DATA_TABLE_STOP
 * 		TEXT_DEFAULT
 * 	TEXT_STOP(flash)
 * 
 * 	DEV_DEFAULT(flash)
 * 
 * 	RODATA_START
 * 		RODATA_DEFAULT
 * 	RODATA_STOP(flash)
 * 
 * 	DATA_START
 * 		DATA_DEFAULT
 * 	DATA_STOP(sram, flash)
 * 
 * 	BSS_START
 * 		BSS_DEFAULT
 * 	BSS_STOP(sram)
 * 
 * 	HEAP(sram, CONFIG_MACH_INIT_STACK_SIZE)
 * 
 * 	STACK(sram)
 * SECTIONS_STOP
 * \endcode
 * \{
 */
#ifdef LINKER_SCRIPT
/**
 * Define a Symbol
 * \param name Symbolname 
 */
#define SYMBOL(name) name = .
/**
 * Define a Memory Block
 */
#define MEM_START MEMORY {
/**
 * Add a Memory Entry 
 * \param name Memory Name like sram or flash
 * \param origin Memory position  
 * \param len Length of Memory Entry 
 */
#define MEM_ADD(name, origin, len) name : ORIGIN = origin, LENGTH = len
/**
 * Stop Memory Block  
 */
#define MEM_STOP }
/**
 * Start Sections Block
 */
#define SECTIONS_START SECTIONS {
/**
 * Start Section
 * \param name Sectionname
 */
#define SECTION_START(name) name : {
#ifdef CONFIG_MACH_HAS_NO_LOCATION
# define SECTION_STOP(location) }
#else
/**
 * Stop Section
 * \param location location
 */
# define SECTION_STOP(location) } > location AT > location
#endif
/**
 * Stop Section located in RAM 
 * \param location Location
 * \param flashLocation Location in flash
 */
#define SECTION_STOP_RAM(location, flashLocation) } > location AT > flashLocation
/**
 * Stop Section
 */
#define SECTIONS_STOP }
/**
 * Start vector Section
 */
#define VECTOR_START SECTION_START(.vectors) . = ALIGN(4);
/**
 * Define entry in Vector section
 */
#define VECTOR_DEFAULT *(.vectors)
/**
 * Stop Vector Section
 * \param location Location
 */
#define VECTOR_STOP(location) SECTION_STOP(location)

/**
 * Start Text Section
 */
#define TEXT_START SECTION_START(.text) . = ALIGN(4); SYMBOL(_start_text);
/**
 * Default Entry for text section
 */
#define TEXT_DEFAULT *(.text*) *(.init*) *(.fini) *(.eh_frame)
/**
 * FreeRTOS Text Section
 */
#define TEXT_FREERTOS *(.text.freertos*)
/**
 * Stop Text Section
 * \param location Location
 */
#define TEXT_STOP(location) SYMBOL(_end_text); SECTION_STOP(location)

#ifdef CONFIG_GENERATE_UNWIND_TABLES
# define UNWIND_TABLES SYMBOL(__exidx_start); *(.ARM.exidx* .gnu.linkonce.armexidx.*) SYMBOL(__exidx_end);
#else
# define UNWIND_TABLES
#endif

/**
 * Start rodata Section
 */
#define RODATA_START SECTION_START(.rodata) . = ALIGN(4);
/**
 * Default Entry in rodata
 */
#define RODATA_DEFAULT *(.rodata) *(.rodata*)
/**
 * Stop rodata Section
 * And add Unwind Tables after rodata Section
 */
#define RODATA_STOP(location) SECTION_STOP(location) SECTION_START(.unwind) UNWIND_TABLES SECTION_STOP(location)

/**
 * start Data Table Section
 */
#define DATA_TABLE_START SYMBOL(_data_table);
#ifndef CONFIG_ARCH_64BIT
/**
 * Add a Entry in a data table
 * \param sname Name of data section for example .data or .data.freertos
 */
#define DATA_TABLE(sname) \
	LONG(LOADADDR(sname)); \
	LONG(ADDR(sname)); \
	LONG(SIZEOF(sname));
#else
/**
 * Add a Entry in a data table
 * \param sname Name of data section for example .data or .data.freertos
 */
#define DATA_TABLE(sname) \
	QUAD(LOADADDR(sname)); \
	QUAD(ADDR(sname)); \
	QUAD(SIZEOF(sname));
#endif
/**
 * Stop Data Table Section
 */
#define DATA_TABLE_STOP SYMBOL(_data_table_end);
/**
 * Start Data Section
 * This Maco define the symbol _start_data
 */
#define DATA_START SECTION_START(.data) . = ALIGN(4); SYMBOL(_start_data);
/**
 * Default in Data Section
 */
#define DATA_DEFAULT \
	*(.data) \
	*(.data*) \
	*(.fini_array) \
	*(.gnu.linkonce.d.*) \
	. = ALIGN(8); \
	__sdata_begin__ = .; \
	*(.srodata.cst16) \
	*(.srodata.cst8) \
	*(.srodata.cst4) \
	*(.srodata.cst2) \
	*(.srodata .srodata.*) \
	*(.sdata .sdata.* .gnu.linkonce.s.*)

/**
 * FreeRTOS Data Section
 */
#define DATA_FREERTOS *(.data.freertos*)
/**
 * Stop Data Section 
 * this Macro define the symbol _end_data
 * \param location Location
 * \param flashLocation Location in flash
 */
#define DATA_STOP(location, flashLocation) SYMBOL(_end_data); SECTION_STOP_RAM(location, flashLocation)

/**
 * Start BSS Section 
 * This Macro define the symbol __bss_start__
 */
#define BSS_START SECTION_START(.bss) . = ALIGN(4); SYMBOL(__bss_start__); 
/**
 * Default BSS Section entrys
 */
#define BSS_DEFAULT \
	*(.dynsbss) \
	*(.sbss .sbss.* .gnu.linkonce.sb.*) \
	*(.scommon) \
	*(.bss) \
	*(.bss*)\
	PROVIDE(__global_pointer$ = MIN(__sdata_begin__ + 0x800, \
		            MAX(_start_data + 0x800, __bss_end__ - 0x800)));
/**
 * FreeRTOS BSS Section
 */
#define BSS_FREERTOS *(.bss.freertos*)
/**
 * Stop BSS Section 
 * This Macro define the symbol __bss_end__
 * \param location Location
 */
#define BSS_STOP(location) SYMBOL(__bss_end__); SECTION_STOP(location)
/**
 * Define Heap Section 
 * \param location Location
 * \param stakSize Stack Size
 */
#define HEAP(location, stakSize) SECTION_START(.heap) \
	. = ALIGN(4); \
	SYMBOL(_heap_end); \
	. = ((ORIGIN(location) + LENGTH(location)) - (stakSize)); \
	. = ALIGN(4); \
	_heap_top = . - 4; \
	SECTION_STOP(location)
/**
 * Define Stack Section 
 * \param location Location
 */
#define STACK(location) SECTION_START(.stackArea) \
	. = ALIGN(4); \
	SYMBOL(_start_stack); \
	_end_stack = (ORIGIN(location) + LENGTH(location)); \
	__freertos_irq_stack_top = (ORIGIN(location) + LENGTH(location)); \
	SECTION_STOP(location)
/**
 * End Symbol
 */
#define END = SYMBOL(_end); PROVIDE(end = .);

#endif
/**\}*/
#endif
