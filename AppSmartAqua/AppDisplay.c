/*
 * AppDisplay.c
 *
 *  Created on: 05/10/2019
 *      Author: evandro
 */
#include "AppDisplay.h"

/*
typedef enum
{
	smart = 0,
	aquarium,
	controll,
	temperature,
	RL1,
	RL2,
	RL3,
	SD1,
	number_max
}index_display_txt_t;
*/
/*
 *
 */
const char *display_txt_en[number_max] =
{
	"SMART",
	"AQUARIUM",
	"CONTROLL",
	"TEMPERATURE",
	"RL1",
	"RL2",
	"RL3",
	"SD1",
	"OFF",
	"ON",
	"ADJUST",
	"MANUAL",
	"AUTO",
	"CONFIG.",
	"DATE",
	"TIME",
	"VIEW",
	"TELEMETRY",
	"RL",
	"TEMPER.",
	"DC",
	"DUTY",
	"CYCLE",
	"MIN.",
	"AVG.",
	"MAX.",
	"DAY",
	"HOUR"
};

/**
 *
 */
static QueueHandle_t display_queue = NULL;


/**
 *
 */
static void app_display_task(void *pvParameters);

/**
 *
 */
void app_display_init_task(void)
{
	if(xTaskCreate(app_display_task,"LCD_task",APP_DISPLAY_TASK_STATCK, NULL, APP_PRIORITIES_HIGH, NULL) != pdPASS)
	{
#if APP_DISPLAY_TASK_DEBUG
		PRINTF("\n\r[APPDISPLAY]Display_task Fail\n\r");
#endif /* APP_DISPLAY_TASK_DEBUG */

	}
}
/**
 *
 */
void app_display_queue_create(void)
{
	display_queue = xQueueCreate(APP_DISPLAY_MAX_LENGTH,sizeof(data_display_t) );
	if(display_queue)
	{
#if APP_DISPLAY_TASK_DEBUG
	PRINTF("\n\r[APPDISPLAY]Create Queue Display ");
#endif	/* APP_DISPLAY_TASK_DEBUG */
	}
}

/**
 *
 */
bool app_display_queue_send(data_display_t data, uint32_t time)
{
	bool ret = true;
	if( xQueueSend( display_queue, ( void * ) &data, ( TickType_t ) time ) != pdPASS )
	{
#if APP_DISPLAY_TASK_DEBUG
	PRINTF("\n\r[APPDISPLAY]Queue Send Fail");
#endif	/* APP_DISPLAY_TASK_DEBUG */
		ret = false;
	}

	return ret;
}

/**
 *
 */
static void app_display_task(void *pvParameters)
{
#if APP_DISPLAY_TASK_DEBUG
		PRINTF("\n\r[APPDISPLAY]Display Task");
#endif /* APP_DISPLAY_TASK_DEBUG */

	data_display_t data;// = {{0},{0}};
	app_lcd_init();

	while(1)
	{
		if( xQueueReceive(display_queue,&data,(TickType_t) 0xFFFFFFFF) == pdTRUE )
		{
#if APP_DISPLAY_TASK_DEBUG
							/*1234567890123456*/
	PRINTF("\n\r[APPDISPLAY] |----------------| ");
	PRINTF("\n\r[APPDISPLAY] |%s|",data.line1);
	PRINTF("\n\r[APPDISPLAY] |%s|",data.line2);
	PRINTF("\n\r[APPDISPLAY] |----------------| ");
	PRINTF("\n\r[APPDISPLAY] Numeber line: %d",data.print);
#endif /* APP_DISPLAY_TASK_DEBUG */

			app_lcd_write_string(line1 ,data.line1);
			app_lcd_write_string(line2 ,data.line2);

		}
		//vTaskDelay(10000);
		//app_lcd_write_string(line1 ,"1234567890123456");
		//app_lcd_write_string(line2 ,"1234567890123456");
		//vTaskDelay(100);
	}
}


