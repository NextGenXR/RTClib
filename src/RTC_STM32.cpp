/*
 * RTC_STM32.cpp
 *
 *  Created on: Oct 26, 2022
 *      Author: joconnor
 */

#include <cstdio>
#include <cstdint>
#include <cstdbool>


#ifdef USE_HAL_DRIVER
#include <stm32yyxx_hal_conf.h>
#include <stm32yyxx_hal_def.h>
#include <stm32yyxx_hal_rtc.h>
#include <rtc.h>
#endif

#include "RTClib.h"


bool RTC_STM32::begin()
{
	// TODO:
	// Determine if clock is inaccurate or lost power, and restore if you can
	// (May need to do that in the backup config routine)


	return (true);
}

bool RTC_STM32::adjust(const DateTime &dt)
{
	RTC_DateTypeDef sdatestructure;
	RTC_TimeTypeDef stimestructure;

	sdatestructure.Year = dt.year();
	sdatestructure.Month = dt.month();
	sdatestructure.Date = dt.day();
	stimestructure.Hours = dt.hour();
	stimestructure.Minutes = dt.minute();
	stimestructure.Seconds = dt.second();

	/* Set the RTC current Time */
	auto result1 = HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
	/* Set the RTC current Date */
	auto result2 = HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);

	return (result1 == HAL_OK && result2 == HAL_OK);
}

bool RTC_STM32::adjust(unsigned long seconds)
{
	return (false);
}

bool RTC_STM32::isrunning(void)
{

	return (true);
}

DateTime RTC_STM32::now()
{

	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

	return (DateTime(sdatestructureget.Year,
	sdatestructureget.Month,
	sdatestructureget.Date,
	stimestructureget.Hours,
	stimestructureget.Minutes,
	stimestructureget.Seconds
	));
}

bool RTC_STM32::now(RTC_DateTypeDef *sdatestructureget, RTC_TimeTypeDef *stimestructureget)
{
	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, stimestructureget, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, sdatestructureget, RTC_FORMAT_BIN);

	return (true);
}


/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void RTC_STM32::RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

	/* Display time Format : hh:mm:ss */
	sprintf((char *)showtime, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
	/* Display date Format : mm-dd-yy */
	sprintf((char *)showdate, "%2d-%2d-%2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
}


// Use a battery-backed hardware RTC as a backup clock.
// Set any time the main clock is set
bool RTC_STM32::backup_rtc(RTC_Type rtc, I2C_HandleTypeDef * Handle, I2C_AddressTypeDef addr)
{

	return (true);
}

bool RTC_STM32::restore_backup(void)
{

	return (true);
}

// TODO: Look at what actual code needs to be for Alarm support
bool RTC_STM32::setAlarm1(const DateTime &dt, STM32Alarm1Mode alarm_mode)
{

	return (true);
}

bool RTC_STM32::setAlarm2(const DateTime &dt, STM32Alarm1Mode alarm_mode)
{

	return (true);
}

void RTC_STM32::disableAlarm(uint8_t alarm_num)
{

}

void RTC_STM32::clearAlarm(uint8_t alarm_num)
{

}

bool RTC_STM32::alarmFired(uint8_t alarm_num)
{
	return (true);
}


