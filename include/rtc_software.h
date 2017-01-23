#ifndef RTC_SOFTWARE_H_
#define RTC_SOFTWARE_H_
#include <timer.h>
/**\cond INTERNAL*/
#include <rtc.h>
#define RTC_PRV
#include <rtc_prv.h>
struct rtc_software {
	struct rtc_generic gen;
	struct timer *timer;
	time_t sec;
};
/**\endcond*/
int32_t rtc_software_connect(struct rtc *r, struct timer *timer);
#define ADD_RTC_SOFTWARE(id) \
	struct rtc_software rtc_software_##id = { \
		RTC_INIT_DEV(software) \
		HAL_NAME("Software RTC "  #id) \
	}; \
	RTC_ADDDEV(software, rtc_software_##id)
#define RTC_SOFTWARE_ID(id) HAL_GET_ID(rtc, software, rtc_software_##id)
#endif
