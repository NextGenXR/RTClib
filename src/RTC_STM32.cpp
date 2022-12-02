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

bool RTC_STM32::begin() {
	// TODO:
	// Determine if clock is inaccurate or lost power, and restore if you can
	// (May need to do that in the backup config routine)

	// ToDo: Determine if RTC is already initialized and skip
#ifdef USE_RTC_LL
	if(true)
	{
		  /*##-1- Configure the RTC peripheral #######################################*/
		  /* Configure RTC prescaler and RTC data registers */
		  /* RTC configured as follows:
			  - Hour Format    = Format 24
			  - Asynch Prediv  = Value according to source clock
			  - Synch Prediv   = Value according to source clock
			  - OutPut         = Output Disable
			  - OutPutPolarity = High Polarity
			  - OutPutType     = Open Drain */
		  RtcHandle.Instance = &hrtc;
		  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
		  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
		  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
		  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
		  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
		  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

		  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
		  {
			/* Initialization Error */
			Error_Handler();
		  }

		/*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
		/* Read the Back Up Register 1 Data */
		if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
		{
			/* Configure RTC Calendar */
			RTC_CalendarConfig();
		}
		else
		{
			/* Check if the Power On Reset flag is set */
			if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
			{
			  /* Turn on LED2: Power on reset occurred */
			  // BSP_LED_On(LED2);
			}
			/* Check if Pin Reset flag is set */
			if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
			{
			  /* Turn on LED1: External reset occurred */
			  // BSP_LED_On(LED1);
			}
			/* Clear source Reset Flag */
			__HAL_RCC_CLEAR_RESET_FLAGS();
		 }
	}
#endif

#ifdef INIT_RTC_HARDWARE

	init();
#endif

	return (true);
}

/**
 * @brief RTC init function
 */
void RTC_STM32::init(void) {
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	/* 32.768kHz LSE clock input */
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
#ifdef RTC_OUTPUT_REMAP_NONE
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
#endif
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief  Configure the current time and date.
 * @param  None
 * @retval None
 */
void RTC_STM32::CalendarConfig(void) {
	RTC_DateTypeDef sdatestructure;
	RTC_TimeTypeDef stimestructure;

	/*##-1- Configure the Date #################################################*/
	/* Set Date: Tuesday February 18th 2023 */
	sdatestructure.Year = 0x17;
	sdatestructure.Month = RTC_MONTH_JUNE;
	sdatestructure.Date = 23;
	sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

	if (HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}

	/*##-2- Configure the Time #################################################*/
	/* Set Time: 02:00:00 */
	stimestructure.Hours = 0x02;
	stimestructure.Minutes = 0x00;
	stimestructure.Seconds = 0x00;
	stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
	stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BCD) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}

	/*##-3- Writes a data in a RTC Backup data Register1 #######################*/
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
}

bool RTC_STM32::adjust(const DateTime &dt) {
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

bool RTC_STM32::adjust(unsigned long seconds) {
	// TODO
	return (false);
}

bool RTC_STM32::isrunning(void) {
	// TODO
	return (true);
}

DateTime RTC_STM32::now() {
	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

	return (DateTime(sdatestructureget.Year, sdatestructureget.Month,
			sdatestructureget.Date, stimestructureget.Hours,
			stimestructureget.Minutes, stimestructureget.Seconds));
}

bool RTC_STM32::now(RTC_DateTypeDef *sdatestructureget,
		RTC_TimeTypeDef *stimestructureget) {
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
void RTC_STM32::CalendarShow(uint8_t *showtime, uint8_t *showdate) {
	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

	/* Display time Format : hh:mm:ss */
	sprintf((char*) showtime, "%2d:%2d:%2d", stimestructureget.Hours,
			stimestructureget.Minutes, stimestructureget.Seconds);
	/* Display date Format : mm-dd-yy */
	sprintf((char*) showdate, "%2d-%2d-%2d", sdatestructureget.Month,
			sdatestructureget.Date, 2000 + sdatestructureget.Year);
}

// Use a battery-backed hardware RTC as a backup clock.
// Set any time the main clock is set
bool RTC_STM32::backup_rtc(RTC_Type rtc, I2C_HandleTypeDef *Handle,
		I2C_AddressTypeDef addr) {

	return (true);
}

bool RTC_STM32::restore_backup(void) {

	return (true);
}

// TODO: Look at what actual code needs to be for Alarm support
bool RTC_STM32::setAlarm1(const DateTime &dt, STM32Alarm1Mode alarm_mode) {
	// HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
	return (true);
}

bool RTC_STM32::setAlarm2(const DateTime &dt, STM32Alarm1Mode alarm_mode) {
	// HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
	return (true);
}

void RTC_STM32::disableAlarm(uint8_t alarm_num) {
	// HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm);
}

void RTC_STM32::clearAlarm(uint8_t alarm_num) {
	// HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm);
}

bool RTC_STM32::alarmFired(uint8_t alarm_num) {
	/* Alarm Reference
	 HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
	 HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
	 HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm);
	 HAL_StatusTypeDef HAL_RTC_GetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Alarm, uint32_t Format);
	 void              HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc);
	 HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
	 void              HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
	 */

	// HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
	return (true);
}

