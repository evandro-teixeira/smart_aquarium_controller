/*
 * AppTemperature.h
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPTEMPERATURE_H_
#define APPSMARTAQUA_APPTEMPERATURE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "AppSmartAquaConfig.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"

typedef struct
{
	int16_t temperature_avg;
	int16_t temperature_min;
	int16_t temperature_max;
}temperature_day_t;

void app_temperature_init_task(void);
void app_temperature_queue_create(void);
uint8_t app_temperature_day_get(void);
uint8_t app_temperature_hour_get(void);
bool app_tempeature_telematic_day_get(uint8_t day,temperature_day_t *temp);
bool app_temperature_telematic_hour_get(uint8_t hour, int16_t *temp);
int16_t app_temperature_get(void);

#endif /* APPSMARTAQUA_APPTEMPERATURE_H_ */
