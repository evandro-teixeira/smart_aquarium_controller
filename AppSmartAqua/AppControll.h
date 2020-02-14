/*
 * AppManager.h
 *
 *  Created on: 13/10/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPCONTROLL_H_
#define APPSMARTAQUA_APPCONTROLL_H_


#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "AppIO.h"
#include "AppSmartAquaConfig.h"

typedef enum
{
	variable_time = 0,
	variable_temp
}variable_type_t;

//typedef enum
//{
//	drive_auto = 0,
//	drive_manual
//}drive_type_t;

typedef struct
{
//	drive_type_t drive;
	variable_type_t var;
	uint8_t relay;
	uint8_t hour_on;
	uint8_t hour_off;
	uint8_t minute_on;
	uint8_t minute_off;
	int16_t temperature_on;
	int16_t temperature_off;
	bool status;
}manager_relay_t;

typedef struct
{
	uint8_t hour_on;
	uint8_t hour_off;
	uint8_t minute_on;
	uint8_t minute_off;
	uint8_t duty_cycle;
	bool status;
}manager_sd_t;

typedef enum
{
	sd_off = 0,
	sd_turning_off,
	sd_turning_on,
	sd_on,
	sd_idle
}state_sd_t;

void app_controll_init_task(void);
void app_manager_config_relay_set(uint8_t relay, manager_relay_t param);
manager_relay_t app_manager_config_relay_get(uint8_t relay);
//io_status_t app_manager_status_relay_get(uint8_t relay); //[APP_NUMBER_OF_RELAY] = {0};

void app_manager_config_sd_set(manager_sd_t param);
manager_sd_t app_manager_config_sd_get(void);
state_sd_t app_manager_state_sd_get(void);// = sd_off;

#endif /* APPSMARTAQUA_APPCONTROLL_H_ */
