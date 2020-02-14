/*
 * AppRtc.c
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */

#include "AppRtc.h"

/**
 *
 */
static rtc_datetime_t date;
static void app_rtc_irq(void);

/**
 *
 */
static void app_rtc_irq(void)
{
    /* Get date time */
    RTC_GetDatetime(&date);
    /* Set alarm time in seconds */
    RTC_SetAlarm(1);
}

/**
 *
 */
void app_rtc_init(void)
{
    rtc_config_t rtcConfig;

    /* Init RTC */
    /*
     * config->clockSource = kRTC_BusClock;
     * config->prescaler = kRTC_ClockDivide_16_2048;
     * config->time_us = 1000000U; ~ 1 secunds
     */
    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);

    /* Set a start date time and start RT */
    date.year = 2019U;
    date.month = 1U;
    date.day = 1U;
    date.hour = 0U;
    date.minute = 0;
    date.second = 0;

    /* Set RTC time to default */
    RTC_SetDatetime(&date);

    RTC_SetAlarmCallback(app_rtc_irq);
    /* Enable RTC Interrupt */
    RTC_EnableInterrupts(RTC, kRTC_InterruptEnable);
    NVIC_EnableIRQ(RTC_IRQn);
    /* Set alarm time in seconds */
    RTC_SetAlarm(1);
}

/**
 *
 */
rtc_datetime_t app_rtc_datetime_get(void)
{
	return date;
}

/**
 *
 */
void app_rtc_update(rtc_datetime_t date)
{
	RTC_Deinit(RTC);
    rtc_config_t rtcConfig;
    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);
	RTC_SetDatetime(&date);
    /* Set alarm time in seconds */
    RTC_SetAlarm(1);
}
