#include "auxiliary/auxiliary.h"

void main(void)
{
	init();

	TC_init(TC0, ID_TC0, 0, 10);
	tc_start(TC0, 0);

	Calendar rtc_initial = {2018, 3, 19, 12, 15, 45, 1};
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_SR_SEC | RTC_SR_ALARM);

	uint32_t current_hour, current_min, current_sec;
	uint32_t current_year, current_month, current_day, current_week;
	rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
	rtc_get_date(RTC, &current_year, &current_month, &current_day, &current_week);

	while (1)
	{
		if (get_rtc_second_flag())
		{
			rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
			draw_time(current_hour, current_min, current_sec);
		}
		if (get_tc0_flag())
		{
			set_tc0_flag(0);
			pin_toggle(LED2_PIO, LED2_PIO_IDX_MASK);
		}

		if (get_but1_flag())
		{
			set_but1_flag(0);
			pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
			RTT_init(4, 16, RTT_MR_ALMIEN);
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}