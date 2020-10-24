obj-y += hal.o
obj-$(CONFIG_NEWLIB) += newlib/
obj-$(CONFIG_GPIO) += gpio/
obj-$(CONFIG_UART) += uart/
obj-$(CONFIG_BUFFER) += buffer/
obj-$(CONFIG_SPI) += spi/
obj-$(CONFIG_SENSOR) += sensor/
obj-$(CONFIG_ADC) += adc/
obj-$(CONFIG_TIMER) += timer/
obj-$(CONFIG_PWM) += pwm/
obj-$(CONFIG_CAPTURE) += capture/
obj-$(CONFIG_EXAMPLE) += example/
obj-$(CONFIG_NLIBC) += nlibc/
obj-$(CONFIG_SD) += sd/
obj-$(CONFIG_MAILBOX) += mailbox/
obj-$(CONFIG_RPROC) += remoteproc/
obj-$(CONFIG_NET) += net/
obj-$(CONFIG_COUNTER) += counter/
obj-$(CONFIG_RTC) += rtc/
obj-y += power/
obj-$(CONFIG_GCC) += gcc/
obj-$(CONFIG_CAN) += can/
obj-$(CONFIG_I2C) += i2c/

obj-$(CONFIG_GEN_VERSION) += version.o
BUILDID := $(USER)@$(shell hostname) $(shell date +'%Y-%m-%d %H:%M:%S')
$(obj)/version.c: FORCE
	@echo "  GEN     $(obj)/version.c"
	$(Q)version="const char *versionDriver = \""; \
	dir=`pwd`; \
	cd `dirname $@`; \
	version=$$version`git log -n 1 --pretty="format:$(BUILDID) %H" --no-notes`; \
	if git diff-index --name-only HEAD | grep -qv "^scripts/package" > /dev/null; then \
		version="$$version-dirty"; \
	fi; \
	version="$$version\";"; \
	cd $$dir; \
	echo $$version > $@;
