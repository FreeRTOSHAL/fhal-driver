#ifndef MCP492X_H_
#define MCP492X_H_
/**\cond INTERNAL*/
#include <dac.h>
#define DAC_PRV
#include <dac_prv.h>
#include <spi.h>
#include <FreeRTOS.h>
#include <task.h>
extern const struct dac_ops mcp492x_dac_ops;
struct dac_mcp492x_controller {
	struct hal gen;
	struct dac_mcp492x *channels[2];
	struct spi_slave *slave;
	struct gpio_pin *lvdac;
};
struct dac_mcp492x {
	struct dac_generic gen;
	struct dac_mcp492x_controller *dacc;
	uint32_t bits;
	uint32_t channelID;
	void *data;
};

#define ADD_MCP492X_CHANNEL(ID, _channelID) \
	struct dac_mcp492x mcp492x_##ID##_##_channelID = { \
		DAC_INIT_DEV(mcp492x) \
		HAL_NAME("TI MCP492x " #ID "Channel " #_channelID) \
		.dacc = &mcp492x_##ID, \
		.channelID = _channelID, \
	};\
	DAC_ADDDEV(mcp492x, mcp492x_##ID##_##_channelID)
/**\endcond*/

void *mcp492x_controller_init(uint32_t index);
int32_t mcp492x_connect(void *ac, struct spi *spi, uint8_t cs, uint16_t gpio, uint16_t lvdac, uint32_t baudrate);

#define ADD_MCP492X(ID) \
	extern struct dac_mcp492x_controller mcp492x_##ID;\
	ADD_MCP492X_CHANNEL(ID, 0); \
	ADD_MCP492X_CHANNEL(ID, 1); \
	struct dac_mcp492x_controller mcp492x_##ID = { \
		HAL_NAME("TI MCP492x " #ID) \
		.channels = { \
			&mcp492x_##ID##_0, \
			&mcp492x_##ID##_1, \
		}, \
	}; \
	HAL_ADDDEV(hal, mcp492x, mcp492x_##ID)

#define MCP492X_ID(ID) HAL_GET_ID(hal, mcp492x, mcp492x_##ID)
#define MCP492X_ID_CHANNEL(ID, _channelID) HAL_GET_ID(dac, mcp492x, mcp492x_##ID##_##_channelID)
#endif

