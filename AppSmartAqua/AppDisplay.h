/*
 * AppDisplay.h
 *
 *  Created on: 05/10/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPDISPLAY_H_
#define APPSMARTAQUA_APPDISPLAY_H_

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "AppSmartAquaConfig.h"

typedef enum
{
	smart = 0,
	aquarium,
	controll,
	tempe,
	RL1,
	RL2,
	RL3,
	SD1,
	off,
	on,
	adjust,
	manual,
	auto_,
	config,
	date_,
	time_,
	view,
	telemety,
	RL,
	temper,
	dc,
	duty,
	cycle,
	min,
	avg,
	max,
	day,
	hour,
	number_max
}index_display_txt_T;

typedef enum
{
	line1 = 0,
	line2,
	all
}print_line_t;

typedef struct
{
	print_line_t print;
	char line1[17/*APP_SIZE_BUFFER_LCD*/];
	char line2[17/*APP_SIZE_BUFFER_LCD*/];
}data_display_t;

extern const char *display_txt_en[number_max];

void app_display_init_task(void);
void app_display_queue_create(void);
bool app_display_queue_send(data_display_t data, uint32_t time);

#endif /* APPSMARTAQUA_APPDISPLAY_H_ */
