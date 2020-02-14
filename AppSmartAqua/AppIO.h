/*
 * AppIO.h
 *
 *  Created on: 21/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPIO_H_
#define APPSMARTAQUA_APPIO_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "AppSmartAquaConfig.h"
#include "fsl_kbi.h"
#include "fsl_ftm.h"

typedef union
{
	uint8_t data;
	struct
	{
		unsigned down: 	1;
		unsigned up: 	1;
		unsigned left: 	1;
		unsigned right:  1;
		unsigned enter:  1;
		unsigned back: 	1;
		unsigned nc: 	2;
	}key;
}io_data_t;

typedef enum
{
	key_down = 0,
	key_up,
	key_left,
	key_right,
	key_enter,
	key_back,
	key_max
}io_key_t;

typedef enum
{
	io_off = false,
	io_on = true,
	io_idle
}io_status_t;

typedef struct
{
	io_status_t led_red;
	io_status_t led_green;
	io_status_t led_blue;
	io_status_t relay1;
	io_status_t relay2;
	io_status_t relay3;
}io_param_t;

typedef enum
{
	RGB_Red = 0,
	RGB_Yellow,
	RGB_Green,
	RGB_Cyan,
	RGB_Blue,
	RGB_Magenta,
	RGB_Write,
	RGB_Off,
	RGB_WithotColor /* Fail */
}rgb_state_t;

/*typedef struct
{
	bool flag;
	io_bit_t ch;
}io_data_t;*/

//typedef struct
//{
//	bool state;
//	bool value;
//	bool value_old;
//	uint32_t counter;
//	uint32_t counter_old;
//}io_debounce_t;

typedef struct
{
	io_data_t io_data;
	uint32_t time;
}io_irq_data_t;

typedef struct
{
	bool state;
	uint32_t time;
}io_ctrl_t;

/**
 *
 */
typedef struct
{
	gpio_port_num_t port;
	uint32_t pin;
}io_pins_t;


void app_io_init(void);
void app_io_init_task(void);
void app_io_queue_create(void);
bool app_io_queue_receive(io_data_t *data, uint32_t time);

io_param_t app_io_status_get(void);

void app_io_relay1_set(io_status_t st);
void app_io_relay2_set(io_status_t st);
void app_io_relay3_set(io_status_t st);

io_status_t app_io_relay1_get(void);
io_status_t app_io_relay2_get(void);
io_status_t app_io_relay3_get(void);

void app_io_ticks_irq(void);

#endif /* APPSMARTAQUA_APPIO_H_ */
