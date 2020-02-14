/*
 * AppShell.c
 *
 *  Created on: 20/09/2019
 *      Author: evandro
 */
#include "AppShell.h"
#include <stdarg.h>

/**
 *
 */
typedef enum
{
	ok = 0,
	wait,
	fail
}st_shell_t;

/**
 *
 */
typedef enum
{
	help = 0,
	clear,
	date,
	time,
	info,
	config_date,
	config_time,
	temperature,
	set_relay1,
	get_relay1,
	set_relay2,
	get_relay2,
	set_relay3,
	get_relay3,
	set_sdigital,
	get_sdigital,
	number_command_max
}index_command_t;

typedef enum
{
	_shell,
	_cursor,
	_fail,
	_date,
	_time,
	_command,
	_relay,
	_value,
	number_txt_max
}index_txt_t;

/**
 *
 */
const char *shell_txt[number_txt_max] =
{
	"Shell",
	">>",
	"Fail",
	"Date",
	"Time",
	"Command",
	"Relay",
	"Value",
};

/**
 *
 */
const char *command_txt[number_command_max] =
{/*	"1234567890123456"	*/
	"help",
	"clear",
	"date",
	"time",
	"info",
	"config_date",
	"config_time",
	"temperature",
	"set_relay1",
	"get_relay1",
	"set_relay2",
	"get_relay2",
	"set_relay3",
	"get_relay3",
	"set_sdigital",
	"get_sdigital"
};

/**
 *
 */
const char *param_txt[number_command_max] =
{/*	"1234567890123456"	*/
	" ",
	" ",
	" ",
	" ",
	" ",
	"[aaaa/mm/dd]",
	"[hh:mm]",
	" ",
	"[on|off]",
	" ",
	"[on|off]",
	" ",
	"[on|off]",
	" ",
	"[on|off]:[xx]",
	" ",
};

/**
 *
 */
static void app_shell_task(void *pvParameters);
static void app_shell_uart_init(void);
static void app_shell_uart_string(const char *format,...);
static void app_shell_buffer_clear(uint8_t *buffer);
static st_shell_t app_shell_interpreter(uint8_t ch);
static void app_shell_command(char *arg0, char *arg1);
static bool app_shell_format(uint8_t *buffer);
static bool app_shell_interpreter_time(char *arg,uint8_t *hour,uint8_t *minute);
static bool app_shell_interpreter_date(char *arg,uint8_t *day,uint8_t *mount,uint16_t *year);

/**
 *
 */
static QueueHandle_t shell_queue = NULL;
static uint8_t buffer_rx[APP_SHELL_MAX_SIZE_BUFFER] = {0};
static uint8_t buffer_arg0[APP_SHELL_MAX_SIZE_BUFFER] = {0};
static uint8_t buffer_arg1[APP_SHELL_MAX_SIZE_BUFFER] = {0};


/**
 *
 */
void app_shell_init_task(void)
{
	if(xTaskCreate(app_shell_task,"Shell_task",APP_SHELL_TASK_STATCK, NULL, APP_PRIORITIES_MED, NULL) != pdPASS)
	{
#if APP_SHELL_TASK_DEBUG
		PRINTF("\n\r[APPSHELL]Shell_task\n\r");
#endif /* APP_SHELL_TASK_DEBUG */
	}
}

/**
 *
 */
void app_shell_queue_create(void)
{
	shell_queue = xQueueCreate(APP_SHELL_MAX_LENGTH,sizeof(uint8_t) );
	if(shell_queue)
	{
#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Create Queue Shell ");
#endif	/* APP_SHELL_TASK_DEBUG */
	}
}

/**
 *
 */
static void app_shell_task(void *pvParameters)
{
	uint8_t data = 0;

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Create Shell_task");
#endif /* APP_SHELL_TASK_DEBUG */

	/* Init UART Shell */
	app_shell_uart_init();

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]UART0 Init");
#endif /* APP_SHELL_TASK_DEBUG */

	app_shell_uart_string("\n\rSmart Aqua V2.0");
	app_shell_uart_string("\n\rShell");
	app_shell_uart_string("\n\r>>");

	while(1)
	{
		if(xQueueReceive(shell_queue,&data,portMAX_DELAY) == pdTRUE )
		{

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Data UART RX: %c",data);
#endif /* APP_SHELL_TASK_DEBUG */

			switch(app_shell_interpreter(data) )
			{
				case ok:
					if(strlen((char*)buffer_rx) > 0)
					{
						if(app_shell_format(buffer_rx) == true)
						{

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Arg0: %s",buffer_arg0);
	PRINTF("\n\r[APPSHELL]Arg1: %s",buffer_arg1);
#endif /* APP_SHELL_TASK_DEBUG */

							app_shell_command((char*)buffer_arg0,(char*)buffer_arg1);
						}
						app_shell_buffer_clear(buffer_arg0);
						app_shell_buffer_clear(buffer_arg1);
						app_shell_buffer_clear(buffer_rx);
					}
					app_shell_uart_string("\n\r%s",shell_txt[_cursor]);
				break;
				case wait:
					app_shell_uart_string("%c",data);
				break;
				case fail:
				default:
					app_shell_uart_string("\n\r%s",shell_txt[_fail]);
					app_shell_uart_string("\n\r%s",shell_txt[_cursor]);
					app_shell_buffer_clear(buffer_arg0);
					app_shell_buffer_clear(buffer_arg1);
					app_shell_buffer_clear(buffer_rx);
				break;
			}
		}
	}
}


/*
 *
 */
static void app_shell_uart_init(void)
{
	uart_config_t config;

	/* PTD7, PTd6 is configured as UART2_TX, UART2_RX */
	PORT_SetPinSelect(kPORT_UART2, kPORT_UART2_RXPTD7_TXPTD6);

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = APP_SHELL_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;
    UART_Init(APP_SHELL_UART, &config,APP_SHELL_UART_CLK_FREQ);

    /* Enable RX interrupt. */
    UART_EnableInterrupts(APP_SHELL_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(APP_SHELL_UART_IRQn);
}

/**
 *
 */
static void app_shell_uart_string(const char *format,...)
{
	char serialbuffer[APP_SHELL_MAX_SIZE_BUFFER];
	va_list args;
	uint16_t len;

	va_start(args,format);
	len = vsnprintf((char*)serialbuffer,APP_SHELL_MAX_SIZE_BUFFER,format,args );
	va_end(args);

	UART_WriteBlocking(APP_SHELL_UART,(uint8_t*)serialbuffer,len);
}

/**
 *
 */
static bool app_shell_format(uint8_t *buffer)
{
	uint8_t i = 0;
	uint8_t index = 0;
	uint16_t size = strlen((char*)buffer);

	for(i=0;i<APP_SHELL_MAX_SIZE_BUFFER;i++)
	{
		if(buffer[index] == ' ')
		{
			index++;
			break;
		}
		buffer_arg0[i] = buffer[index++];
		if(index >= APP_SHELL_MAX_SIZE_BUFFER)
		{
			break;
		}
		if(index >= size)
		{

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Format Successfully Concluded");
#endif /* APP_SHELL_TASK_DEBUG */

			return true;
		}
	}

	for(i=0;i<APP_SHELL_MAX_SIZE_BUFFER;i++)
	{
		if(buffer[index] != ' ')
		{
			buffer_arg1[i] = buffer[index++];
			if(index >= APP_SHELL_MAX_SIZE_BUFFER)
			{

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Fail format");
#endif /* APP_SHELL_TASK_DEBUG */

				return false;
			}

			if(index >= size)
			{

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Format Successfully Concluded");
#endif /* APP_SHELL_TASK_DEBUG */

				return true;
			}
		}
		else
		{
			if(index < size)
			{

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Fail format");
#endif /* APP_SHELL_TASK_DEBUG */

				return false;
			}
		}
	}

#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Format finichi");
#endif /* APP_SHELL_TASK_DEBUG */

	return true;
}

/**
 *
 */
static st_shell_t app_shell_interpreter(uint8_t ch)
{
	static uint8_t index = 0;

	if((ch == '\n') || (ch == '\r'))
	{
		index = 0;
#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Interpreter Successfully Concluded");
#endif /* APP_SHELL_TASK_DEBUG */
		return ok;
	}
	else
	{
		if( (ch >= '0' && ch <= '9') ||
			(ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch == ' ') ||
			(ch == '/') ||
			(ch == ':') ||
			(ch == '_'))
		{
			buffer_rx[index++] = ch;
			if(index >= APP_SHELL_MAX_SIZE_BUFFER)
			{
				index = 0;
#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Fail Interpreter");
#endif /* APP_SHELL_TASK_DEBUG */
				return fail;
			}
			return wait;
		}
		else
		{
#if APP_SHELL_TASK_DEBUG
	PRINTF("\n\r[APPSHELL]Fail Interpreter");
#endif /* APP_SHELL_TASK_DEBUG */
			return fail;
		}
	}
}

/**
 *
 */
static void app_shell_buffer_clear(uint8_t *buffer)
{
	uint8_t i = 0;
	for(i=0;i<APP_SHELL_MAX_SIZE_BUFFER;i++)
	{
		buffer[i] = 0;
	}
}

/**
 *
 */
void APP_SHELL_IRQHandler(void)
{
	uint8_t data;

    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(APP_SHELL_UART))
    {
		data = UART_ReadByte(APP_SHELL_UART);
		/* Send msg with status RX UART */
		xQueueSendToFrontFromISR(shell_queue,&data,pdFALSE);
    }
}

/**
 *
 */
static void app_shell_command(char *arg0, char *arg1)
{
	if(strcmp(arg0,command_txt[help]) == 0)
	{
		uint8_t i = 0;
		app_shell_uart_string("\n\rList Command:");
		for(i=0;i<number_command_max;i++)
		{
			app_shell_uart_string("\n\r%s %s",command_txt[i],param_txt[i]);
		}
		return;
	}
	if(strcmp(arg0,command_txt[clear]) == 0)
	{
		app_shell_uart_string("\e[H\e[2J");
		return;
	}
	if(strcmp(arg0,command_txt[date]) == 0)
	{
		app_shell_uart_string("\n\r%d/%02d/%02d",app_rtc_datetime_get().year,
				app_rtc_datetime_get().month,
				app_rtc_datetime_get().day);
		return;
	}
	if(strcmp(arg0,command_txt[time]) == 0)
	{
		app_shell_uart_string("\n\r%02d:%02d:%02d",app_rtc_datetime_get().hour,
				app_rtc_datetime_get().minute,
				app_rtc_datetime_get().second);
		return;
	}
	if(strcmp(arg0,command_txt[info]) == 0)
	{
		app_shell_uart_string("\n\r%s",APP_VER_FIRMWARE);
		return;
	}
	if(strcmp(arg0,command_txt[config_date]) == 0)
	{
		rtc_datetime_t date = app_rtc_datetime_get();
		if(app_shell_interpreter_date(arg1,&date.day,&date.month,&date.year) == true)
		{
			app_rtc_update(date);
			app_shell_uart_string("\n\r%d/%02d/%02d",date.year,date.month,date.day);
		}
		else
		{
			app_shell_uart_string("\n\r%s %s",shell_txt[_date],shell_txt[_fail]);
		}
		return;
	}
	if(strcmp(arg0,command_txt[config_time]) == 0)
	{
		rtc_datetime_t date = app_rtc_datetime_get();
		if(app_shell_interpreter_time(arg1,&date.hour,&date.minute) == true)
		{
			app_rtc_update(date);
			app_shell_uart_string("\n\r%02d:%02d:%02d",date.hour,date.minute,date.second);
		}
		else
		{
			app_shell_uart_string("\n\r%s %s",shell_txt[_time],shell_txt[_fail]);
		}
		return;
	}

	app_shell_uart_string("\n\r%s %s",shell_txt[_command],shell_txt[_fail]);
}

/**
 *
 */
static bool app_shell_interpreter_time(char *arg,uint8_t *hour,uint8_t *minute)
{
	char ch = 0;
	uint8_t i = 0;
	uint8_t st = 0;
	uint8_t mult = 0;
	uint8_t index = 0;
	uint16_t number = 0;
	uint8_t var_hour = 0, var_minute = 0;
	uint16_t size = strlen((char*)arg);

	for(i=0;i<size;i++)
	{
		ch = arg[index++];

		if(ch == ':')
		{
			mult = 0;
			st++;
		}
		else
		{
			if(ch >= '0' && ch <= '9')
			{
				if(mult == 0)
				{
					mult++;
					number = (ch - '0');
				}
				else
				{
					number *= 10;
					number += (ch - '0');
				}

				switch(st)
				{
					case 0: /* hour */
						var_hour = (uint8_t)number;
					break;
					case 1: /* minute */
						var_minute = (uint8_t)number;
					break;
					default:
						return false;
					break;
				}
			}
			else
			{
				return false;
			}
		}
	}

	if((var_hour > 23) && (var_minute >= 60))
	{
		return false;
	}

	*hour = var_hour;
	*minute = var_minute;

	return true;
}

/**
 *
 */
static bool app_shell_interpreter_date(char *arg, uint8_t *day,uint8_t *mount, uint16_t *year)
{
	char ch = 0;
	uint8_t i = 0;
	uint8_t st = 0;
	uint8_t mult = 0;
	uint8_t index = 0;
	uint16_t number = 0;
	uint8_t var_day = 0, var_mount = 0;
	uint16_t var_year = 0;
	uint16_t size = strlen((char*)arg);

	for(i=0;i<size;i++)
	{
		ch = arg[index++];

		if(ch == '/')
		{
			mult = 0;
			st++;
		}
		else
		{
			if(ch >= '0' && ch <= '9')
			{
				if(mult == 0)
				{
					mult++;
					number = (ch - '0');
				}
				else
				{
					number *= 10;
					number += (ch - '0');
				}

				switch(st)
				{
					case 0: /* year */
						var_year = number;
					break;
					case 1: /* mount */
						var_mount = (uint8_t)(number);
					break;
					case 2: /* day */
						var_day = (uint8_t)(number);
					break;
					default:
						return false;
					break;
				}
			}
			else
			{
				return false;
			}
		}
	}

	if( (var_year < 2000 && var_year > 2100) || (var_mount > 12) )
	{
		return false;
	}

	switch(var_mount)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if((var_day == 0)||(var_day > 31))
			{
				return false;
			}
		break;

		case 2:
			if(var_year % 4)
			{
				if((var_day == 0)||(var_day > 28))
				{
					return false;
				}
			}
			else
			{
				if((var_day == 0)||(var_day > 29))
				{
					return false;
				}
			}
		break;

		case 4:
		case 6:
		case 9:
		case 11:
			if(number > 30)
			{
				return false;
			}
		break;

		default:
			return false;
		break;
	}

	*day = var_day;
	*mount = var_mount;
	*year = var_year;
	return true;
}
