#ifndef CLOCK_H_
#define CLOCK_H_
#include <stdint.h>
/**
 * \defgroup CLOCK Clock Subsystem
 * \ingroup HAL
 * \code
 * #include <accel.h>
 * \endcode
 * 
 * This is the Clock Subsystem.
 * \todo Implement Clock Subsystem for better support for Power Mangement
 * \{
 */
/**
 * Clock Init
 * \return Clock Instance or Null on error
 */
struct clock *clock_init();
/**
 * Deinit Clock
 * \param clk CLock Instance
 * \return -1 on error 0 on ok
 */
int32_t clock_deinit(struct clock *clk);
/**\}*/
#endif
