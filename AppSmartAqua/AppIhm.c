/*
 * AppIhm.c
 *
 *  Created on: 03/10/2019
 *      Author: evandro
 */

#include "AppIhm.h"

/**
 *
 */
typedef enum
{
	state_init = 0,
	state_main_status,
	state_main_menu,
	state_config_manual,
	state_config_auto,
	state_config_time,
	state_config_date,
	state_view_telemetry,
	state_view_telemetry_day,
	state_view_telemetry_hour,
	state_config_auto_relay1,
	state_config_auto_relay2,
	state_config_auto_relay3,
	state_config_auto_sd,
	//state_config_auto_relay1_var,
	//state_config_auto_relay2_var,
	//state_config_auto_relay3_var,
	state_config_auto_relay1_time,
	state_config_auto_relay2_time,
	state_config_auto_relay3_time,
	state_config_auto_relay1_temp,
	state_config_auto_relay2_temp,
	state_config_auto_relay3_temp,
	state_config_auto_sd_time_off,
	state_config_auto_sd_time_on,
	state_config_auto_sd_dc,
	number_state_max,
}st_ctrl_t;

/**
 *
 */
typedef struct
{
	bool flag_button;
	io_data_t button;
}data_ctrl_t;

/**
 *
 */
static void app_ihm_task(void *pvParameters);
static void app_ihm_init(void);
static void app_ihm_irq(void);
static uint32_t app_ihm_tick_get(void);
static bool app_ihm_state_init(void);
//static bool app_control_main_status(st_ctrl_t *state_out);
static bool app_ihm_main_status(data_ctrl_t button, st_ctrl_t *state_out);
//static bool app_control_main_menu(st_ctrl_t *state_out);
static bool app_ihm_main_menu(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_adjust_manual(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_config_auto(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_config_time(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_config_date(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_config_auto_relay(data_ctrl_t button,uint8_t relay,st_ctrl_t *state_out);
static bool app_ihm_config_auto_relay_time(data_ctrl_t button,uint8_t relay,st_ctrl_t *state_out);
static bool app_ihm_config_auto_relay_temperature(data_ctrl_t button,uint8_t relay,st_ctrl_t *state_out);
static bool app_ihm_config_auto_sd(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_config_auto_sd_time(data_ctrl_t button,uint8_t cursor,st_ctrl_t *state_out);
static bool app_ihm_config_auto_sd_dc(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_view_telemetry(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_view_telemetry_day(data_ctrl_t button,st_ctrl_t *state_out);
static bool app_ihm_view_telemetry_hour(data_ctrl_t button,st_ctrl_t *state_out);
static void app_ihm_enable_sd_set(uint8_t param);
static bool app_ihm_enable_sd_get_status(void);
/**
 *
 */
static st_ctrl_t state_machine = state_init;
static uint32_t ticks = 0;
static uint8_t enable_sd = 0;
static manager_sd_t data_sd;
/**
 *
 */
void app_ihm_init_task(void)
{
	if(xTaskCreate(app_ihm_task,"Ihm_task",APP_IHM_TASK_STATCK, NULL, APP_PRIORITIES_MED, NULL) != pdPASS)
	{
#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Control_task Fail\n\r");
#endif /* APP_IHM_TASK_DEBUG */
	}
}

/**
 *
 */
static void app_ihm_task(void *pvParameters)
{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Control Task");
#endif /* APP_IHM_TASK_DEBUG */

	app_ihm_init();

	while(1)
	{
		/* variable local */
		data_ctrl_t button = {0};

		/* Chech Queue I/O */
		if(app_io_queue_receive((io_data_t*)&button.button,10) == true)
		{
#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Receive Queue I/O - Button: %d",button.button.data);
#endif /* APP_CONTROL_TASK_DEBUG */
			button.flag_button = true;
		}

		/* App State Machine */
		switch(state_machine)
		{
			case state_init:
				if(app_ihm_state_init() == true)
				{
					state_machine = state_main_status;
				}
			break;
			case state_main_status:
			{
				st_ctrl_t state_next;
				if(app_ihm_main_status(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_main_menu:
			{
				st_ctrl_t state_next;
				if(app_ihm_main_menu(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_manual:
			{
				st_ctrl_t state_next;
				if(app_ihm_adjust_manual(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_auto:
			{
				st_ctrl_t state_next;
				if(app_ihm_config_auto(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_time:
			{
				st_ctrl_t state_next;
				if(app_ihm_config_time(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_date:
			{
				st_ctrl_t state_next;
				if(app_ihm_config_date(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_view_telemetry:
			{
				st_ctrl_t state_next;
				if(app_ihm_view_telemetry(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_view_telemetry_day:
			{
				st_ctrl_t state_next;
				if(app_ihm_view_telemetry_day(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_view_telemetry_hour:
			{
				st_ctrl_t state_next;
				if(app_ihm_view_telemetry_hour(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_auto_relay1:
			case state_config_auto_relay2:
			case state_config_auto_relay3:
			{
				st_ctrl_t state_next;
				uint8_t relay = state_machine - state_config_auto_relay1;
				if(app_ihm_config_auto_relay(button,relay,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_auto_sd:
			{
				st_ctrl_t state_next;
				if(app_ihm_config_auto_sd(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_auto_relay1_time:
			case state_config_auto_relay2_time:
			case state_config_auto_relay3_time:
			{
				st_ctrl_t state_next;
				uint8_t relay = state_machine - state_config_auto_relay1_time;
				if(app_ihm_config_auto_relay_time(button,relay,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_auto_relay1_temp:
			case state_config_auto_relay2_temp:
			case state_config_auto_relay3_temp:
			{
				st_ctrl_t state_next;
				uint8_t relay = state_machine - state_config_auto_relay1_temp;
				if(app_ihm_config_auto_relay_temperature(button,relay,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_auto_sd_time_off:
			case state_config_auto_sd_time_on:
			{
				st_ctrl_t state_next;
				uint8_t cursor = state_machine - state_config_auto_sd_time_off;
				if(app_ihm_config_auto_sd_time(button,cursor,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case state_config_auto_sd_dc:
			{
				st_ctrl_t state_next;
				if(app_ihm_config_auto_sd_dc(button,&state_next) == true)
				{
					state_machine = state_next;
				}
			}
			break;
			case number_state_max:
			default:
				state_machine = state_init;
			break;
		}
	}
}

/**
 *
 */
static void app_ihm_init(void)
{
	app_timer_ftm_init(APP_FTM2_BASEADDR, 1000 /* 1 millissecunds */);
	app_timer_ftm_callback(APP_FTM2_BASEADDR, app_ihm_irq);
	app_timer_ftm_start(APP_FTM2_BASEADDR);
	//app_io_init();
	app_pwm_init();
	app_rtc_init();
}

/**
 *
 */
static void app_ihm_irq(void)
{
	ticks++;
}

/**
 *
 */
static uint32_t app_ihm_tick_get(void)
{
	return ticks;
}

/**
 *
 */
static bool app_ihm_state_init(void)
{
	static uint32_t time = 0;
	static uint8_t state = 0;
	bool ret = false;
	data_display_t data = {0,{0},{0}};

	switch(state)
	{
		case 0:
#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Views Screen Init");
#endif /* APP_IHM_TASK_DEBUG */
			/* Preparada texto para enviar ao display */
			/*1234567890123456*/
			/* SMART AQUARIUM */
			/*  CONTROLL V2.0 */
			sprintf(data.line1," %s %s ",display_txt_en[smart],display_txt_en[aquarium]);
			sprintf(data.line2,"  %s V2.0 ",display_txt_en[controll]);
			data.print = all;
			/* envia texto para */
			app_display_queue_send(data,0);
			time = app_ihm_tick_get();
			state = 1;
		break;
		case 1:
			if(app_ihm_tick_get() > (time + APP_TIME_STATE_INIT))
			{
				ret = true;
				state = 0;
			}
		break;
		default:
			state = 1;
		break;
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_main_status(data_ctrl_t button, st_ctrl_t *state_out)
{
	static uint32_t time = 0;
	static uint8_t state = 0;
	static uint8_t state_next = 0;
	static bool print = false;
	data_display_t data = {0,{0},{0}};
	bool ret = false;

	static unsigned char ch = 165;

	if(button.flag_button == true)
	{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Status Finishi");
#endif /* APP_IHM_TASK_DEBUG */

		ret = true;
		print = false;
		state = 0;
		*state_out = state_main_menu;
		return ret;
	}
	else
	{
		switch(state)
		{
			case 0:

				time = app_ihm_tick_get();
				state_next++;
				if(state_next >= 4)
				{
					state_next = 1;
				}
				state = state_next;
				print = false;

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Status Screen: %d",state);
#endif /* APP_IHM_TASK_DEBUG */

			break;
			case 1: /* print date & time */
				if(app_ihm_tick_get() > (time + APP_TIME_STATE_MAIN_STATUS))
				{
					state = 0;
				}
				else
				{
					if(print == false)
					{
						rtc_datetime_t data_rtc = app_rtc_datetime_get();
						print = true;
						/* Preparada texto para enviar ao display */
						/*1234567890123456*/
						/*   dd/mm/aaaa   */
						/*      HH:MM     */
						sprintf(data.line1,"   %02d/%02d/%d   ",data_rtc.day,data_rtc.month,data_rtc.year);
						sprintf(data.line2,"      %02d:%02d     ",data_rtc.hour,data_rtc.minute);
						data.print = all;
						/* envia texto para */
						app_display_queue_send(data,0);
					}
				}
			break;
			case 2:
				if(app_ihm_tick_get() > (time + APP_TIME_STATE_MAIN_STATUS))
				{
					state = 0;
				}
				else
				{
					if(print == false)
					{
						int16_t temperature = app_temperature_get();
						//temperature = 180; // teste 18,0ºC
						print = true;
						/* Preparada texto para enviar ao display */
						/*1234567890123456*/
						/*  TEMPERATURE   */
						/*     xx.x C     */
						sprintf(data.line1,"  %s   ",display_txt_en[tempe]);
						sprintf(data.line2,"    %02d.%02d%cC     ",temperature/10,temperature%10,ch++);
						data.print = all;
						/* envia texto para */
						app_display_queue_send(data,0);
					}
				}
			break;
			case 3:
				if(app_ihm_tick_get() > (time + APP_TIME_STATE_MAIN_STATUS))
				{
					state = 0;
				}
				else
				{
					if(print == false)
					{
						print = true;
						/* Preparada texto para enviar ao display */
						/*1234567890123456*/
						/*RL1 XXX  RL3 XXX*/
						/*RL2 XXX  SD1 XXX*/
						sprintf(data.line1,"%s:%3s  %s:%3s",display_txt_en[RL1],
								app_io_status_get().relay1 == io_on ? display_txt_en[on] : display_txt_en[off],
								display_txt_en[RL3],
								app_io_status_get().relay3 == io_on ? display_txt_en[on] : display_txt_en[off]);
						//sprintf(data.line2,"%s",temperature/10,temperature%10);
						sprintf(data.line2,"%s:%3s  %s:%3s",display_txt_en[RL2],
								app_io_status_get().relay2 == io_on ? display_txt_en[on] : display_txt_en[off],
								display_txt_en[SD1],
								app_pwm_get().status == true ? display_txt_en[on] : display_txt_en[off]);
						data.print = all;
						/* envia texto para */
						app_display_queue_send(data,0);
					}
				}
			break;
			default:
				state = 0;
			break;
		}
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_main_menu(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint32_t timeout = 0;
	static uint8_t state_timeout = 0;
	//static uint8_t state = 0;
	static uint8_t cursor = 0;
	static bool print = false;
	data_display_t data = {0,{0},{0}};
	bool ret = false;

	if(button.flag_button == true)
	{
		/*  reset timeout */
		state_timeout = 0;

		/* limpa flag print */
		print = false;

		/* Checa Teclas */
		if(button.button.key.up == 1)
		{
			if(cursor <= 5)
			{
				cursor++;
			}
			else
			{
				cursor = 0;
			}
		}
		if(button.button.key.down == 1)
		{
			if(cursor > 0)
			{
				cursor--;
			}
			else
			{
				cursor = 4;
			}
		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Menu Finishi - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			*state_out = state_main_status;
			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Menu Finishi - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			switch(cursor)
			{
				case 0:
					*state_out = state_config_manual;
				break;
				case 1:
					*state_out = state_config_auto;
				break;
				case 2:
					*state_out = state_config_date;
				break;
				case 3:
					*state_out = state_config_time;
				break;
				case 4:
					*state_out = state_view_telemetry;
				break;
				default:
					*state_out = state_main_status;
				break;
			}
			return ret;
		}
	}
	else
	{
		switch(state_timeout)
		{
			case 0:
				timeout = app_ihm_tick_get();
				state_timeout = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout+APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Menu Finishi - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					ret = true;
					print = false;
					state_timeout = 0;
					*state_out = state_main_status;
				}
			break;
			default:
				state_timeout = 0;
			break;
		}
	}

	switch(cursor)
	{
		case 0:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>ADJUST MANUAL */
				/*  ADJUST AUTO   */
				sprintf(data.line1,">>%s %s ",display_txt_en[adjust],display_txt_en[manual]);
				sprintf(data.line2,"  %s %s   ",display_txt_en[adjust],display_txt_en[auto_]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 1:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>ADJUST AUTO   */
				/*  CONFIG. DATE  */
				sprintf(data.line1,">>%s %s   ",display_txt_en[adjust],display_txt_en[auto_]);
				sprintf(data.line2,"  %s %s  ",display_txt_en[config],display_txt_en[date_]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 2:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>CONFIG. DATE  */
				/*  CONFIG. TIME  */
				sprintf(data.line1,">>%s %s  ",display_txt_en[config],display_txt_en[date_]);
				sprintf(data.line2,"  %s %s  ",display_txt_en[config],display_txt_en[time_]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 3:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>CONFIG. TIME  */
				/*  VIEW TELEMETRY*/
				sprintf(data.line1,">>%s %s  ",display_txt_en[config],display_txt_en[time_]);
				sprintf(data.line2,"  %s %s",display_txt_en[view],display_txt_en[telemety]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 4:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*  CONFIG. TIME  */
				/*>>VIEW TELEMETRY*/
				sprintf(data.line1,"  %s %s  ",display_txt_en[config],display_txt_en[time_]);
				sprintf(data.line2,">>%s %s",display_txt_en[view],display_txt_en[telemety]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		default:
		break;
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_adjust_manual(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t cursor = 0;
	static uint8_t cursor_h = 0;
	static bool print = false;
	bool param_state[4] = {false};
	data_display_t data = {0,{0},{0}};
	bool ret = false;
	uint8_t dc_value = app_pwm_get().duty_cycle;
	param_state[0] = app_io_status_get().relay1;
	param_state[1] = app_io_status_get().relay2;
	param_state[2] = app_io_status_get().relay3;
	param_state[3] = app_pwm_get().status;


	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;

		/* clear flag print */
		print = false;

		if(button.button.key.up == true)
		{
			pwm_t st = app_pwm_get();

			if(cursor_h == 0)
			{
				if(cursor < 4)
				{
					cursor++;
				}
				else
				{
					cursor = 0;
				}
			}
			else
			{
				if(dc_value < 100)
				{
					dc_value += 1;
				}
			}
			/* Set Duty Cycle */
			st.duty_cycle = dc_value;
			app_pwm_set(st);
		}
		if(button.button.key.down == true)
		{
			pwm_t st = app_pwm_get();

			if(cursor_h == 0)
			{
				if(cursor > 0)
				{
					cursor--;
				}
				else
				{
					cursor = 3;
				}
			}
			else
			{
				if(dc_value > 0)
				{
					dc_value -= 1;
				}
			}
			/* Set Duty Cycle */
			st.duty_cycle = dc_value;
			app_pwm_set(st);
		}
		if(button.button.key.right == true)
		{
			if(cursor == 3)
			{
				if(cursor_h == 0)
				{
					cursor_h = 1;
				}
				else
				{
					cursor_h = 0;
				}
			}
		}
		if(button.button.key.left == true)
		{
			if(cursor == 3)
			{
				if(cursor_h == 0)
				{
					cursor_h = 1;
				}
				else
				{
					cursor_h = 0;
				}
			}
		}

		if(button.button.key.enter == true)
		{
			if(cursor_h == 0)
			{
				if(param_state[cursor] == false)
				{
					param_state[cursor] = true;
				}
				else
				{
					param_state[cursor] = false;
				}

				switch(cursor)
				{
					case 0:
						app_io_relay1_set( param_state[cursor] );
					break;
					case 1:
						app_io_relay2_set( param_state[cursor] );
					break;
					case 2:
						app_io_relay3_set( param_state[cursor] );
					break;
					case 3:
					{
						pwm_t st = app_pwm_get();
						st.status = param_state[cursor];
						app_pwm_set(st);
					}
					break;
				}
			}
		}
		if(button.button.key.back == true)
		{
#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Adjust Manual - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			timeout_state = 0;
			*state_out = state_main_menu;
			return ret;
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Adjust Manual - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					*state_out = state_main_menu;
					return ret;
				}
			break;
			default:
				timeout = 0;
			break;
		}
	}

	switch(cursor)
	{
		case 0:
		//break;
		case 1:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>RL1: XXX      */
				/*  RL2: XXX      */
				sprintf(data.line1,">>%s: %3s      ",display_txt_en[(RL1+cursor)],
						param_state[cursor] == true ? display_txt_en[on] : display_txt_en[off]);
				sprintf(data.line2,"  %s: %3s      ",display_txt_en[(RL2+cursor)],
						param_state[(cursor+1)] == true ? display_txt_en[on] : display_txt_en[off]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 2:
		//break;
		case 3:
			if(print == false)
			{
				uint8_t cursor_aux = 0;
				char ch = '%';
				print = true;
				if(cursor == 3)
				{
					cursor_aux = cursor - 1;
				}
				else
				{
					cursor_aux = cursor;
				}
				/*1234567890123456*/
				/*>>RL3: XXX      */
				/*  SD1: XXX >>XX%*/
				sprintf(data.line1,"%s%s: %3s      ",cursor == 2 ? ">>" : "  ",
													display_txt_en[RL3],
						param_state[cursor_aux] == true ? display_txt_en[on] : display_txt_en[off]);

				sprintf(data.line2,"%s%s: %3s %s%02d%c",(cursor==3 && cursor_h==0) ? ">>" : "  ",
						display_txt_en[SD1],
						param_state[(cursor_aux+1)] == true ? display_txt_en[on] : display_txt_en[off],
						(cursor==3 && cursor_h==1) ? ">>" : "  ",
						(dc_value < 100) ? (dc_value) : (dc_value - 1),
						ch);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		default:
		break;
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_auto(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint32_t timeout = 0;
	static uint8_t state_timeout = 0;
	static uint8_t cursor = 0;
	static bool print = false;
	data_display_t data = {0,{0},{0}};
	bool ret = false;

	if(button.flag_button == true)
	{
		/*  reset timeout */
		state_timeout = 0;

		/* limpa flag print */
		print = false;

		/* Checa Teclas */
		if(button.button.key.up == 1)
		{
			if(cursor < 3)
			{
				cursor++;
			}
			else
			{
				cursor = 0;
			}
		}
		if(button.button.key.down == 1)
		{
			if(cursor > 0)
			{
				cursor--;
			}
			else
			{
				cursor = 3;
			}
		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Menu Finishi - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			*state_out = state_main_menu;
			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Menu Finishi - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			switch(cursor)
			{
				case 0:
					*state_out = state_config_auto_relay1;
				break;
				case 1:
					*state_out = state_config_auto_relay2;
				break;
				case 2:
					*state_out = state_config_auto_relay3;
				break;
				case 3:
					*state_out = state_config_auto_sd;
				break;
				default:
					*state_out = state_main_menu;
				break;
			}
			return ret;
		}
	}
	else
	{
		switch(state_timeout)
		{
			case 0:
				timeout = app_ihm_tick_get();
				state_timeout = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout+APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Main Menu Finishi - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					ret = true;
					print = false;
					state_timeout = 0;
					*state_out = state_main_menu;
				}
			break;
			default:
				state_timeout = 0;
			break;
		}
	}

	switch(cursor)
	{
		case 0:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>CONFIG. RL1   */
				/*  CONFIG. RL2   */
				sprintf(data.line1,">>%s %s   ",display_txt_en[config],display_txt_en[RL1]);
				sprintf(data.line2,"  %s %s   ",display_txt_en[config],display_txt_en[RL2]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 1:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>CONFIG. RL2   */
				/*  CONFIG. RL3   */
				sprintf(data.line1,">>%s %s   ",display_txt_en[config],display_txt_en[RL2]);
				sprintf(data.line2,"  %s %s  ",display_txt_en[config],display_txt_en[RL3]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 2:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*>>CONFIG. RL3   */
				/*  CONFIG. SD1   */
				sprintf(data.line1,">>%s %s   ",display_txt_en[config],display_txt_en[RL3]);
				sprintf(data.line2,"  %s %s   ",display_txt_en[config],display_txt_en[SD1]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 3:
			if(print == false)
			{
				print = true;
				/*1234567890123456*/
				/*  CONFIG. RL3   */
				/*>>CONFIG. SD1   */
				sprintf(data.line1,"  %s %s   ",display_txt_en[config],display_txt_en[RL3]);
				sprintf(data.line2,">>%s %s   ",display_txt_en[config],display_txt_en[SD1]);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		default:
		break;
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_time(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t state = 0;
	static uint8_t var_control = 0;
	static bool print = false;
	static rtc_datetime_t var_time = {0};
	data_display_t data = {0,{0},{0}};
	bool ret = false;


	if(state == 0)
	{
		var_time = app_rtc_datetime_get();
		state = 1;
	}

	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

		if(button.button.key.up == true)
		{
			if(var_control == 0)
			{
				if(var_time.hour < 23)
				{
					var_time.hour++;
				}
				else
				{
					var_time.hour = 0;
				}
			}
			else
			{
				if(var_time.minute < 59)
				{
					var_time.minute++;
				}
				else
				{
					var_time.minute = 0;
				}
			}
		}
		if(button.button.key.down == true)
		{
			if(var_control == 0)
			{
				if(var_time.hour > 0)
				{
					var_time.hour--;
				}
				else
				{
					var_time.hour = 23;
				}
			}
			else
			{
				if(var_time.minute > 0)
				{
					var_time.minute--;
				}
				else
				{
					var_time.minute = 59;
				}
			}
		}
		if((button.button.key.left == true) || (button.button.key.right == true))
		{
			if(var_control == 0)
			{
				var_control = 1;
			}
			else
			{
				var_control = 0;
			}
		}
//		if(button.button.byte.right == true)
//		{
//
//		}
		if(button.button.key.enter == true)
		{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Config Time - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			rtc_datetime_t time = app_rtc_datetime_get();
			time.hour = var_time.hour;
			time.minute = var_time.minute;
			app_rtc_update(time);

			print = false;
			timeout_state = 0;
			ret = true;
			state = 0;
			*state_out = state_main_menu;
			return ret;
		}
		if(button.button.key.back == true)
		{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPCONTROL]Config Time - Back");
#endif /* APP_IHM_TASK_DEBUG */

			print = false;
			timeout_state = 0;
			ret = true;
			state = 0;
			*state_out = state_main_menu;
			return ret;
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPCONTROL]Config Time - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					state = 0;
					*state_out = state_main_menu;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

//	switch(state)
//	{
//		case 0:
//			var_time = app_rtc_datetime_get();
//			state = 1;
//			if(print == false)
//			{
//				print = true;
//				/*1234567890123456*/
//				/*  CONFIG. TIME  */
//				/*     XX:XX      */
//				sprintf(data.line1,"  %s %s  ",display_txt_en[config],display_txt_en[time_]);
//				sprintf(data.line2,"     %02d:%02d      ",var_time.hour,var_time.hour);
//				data.print = all;
//				/* envia texto para */
//				app_display_queue_send(data,0);
//			}
//		break;
//		case 1:
//		default:
//		break;
//	}

	if(print == false)
	{
		print = true;
		/*1234567890123456*/
		/*  CONFIG. TIME  */
		/*     XX:XX      */
		sprintf(data.line1,"  %s %s  ",display_txt_en[config],display_txt_en[time_]);
		sprintf(data.line2,"     %02d:%02d      ",var_time.hour,var_time.minute);
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_date(data_ctrl_t button,st_ctrl_t *state_out)
{
	bool ret = false;
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t state = 0;
	static uint8_t var_control = 0;
	static bool print = false;
	static rtc_datetime_t var_time = {0};
	data_display_t data = {0,{0},{0}};
//	bool ret = false;

	if(state == 0)
	{
		var_time = app_rtc_datetime_get();
		state = 1;
	}

	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

		if(button.button.key.up == true)
		{
			switch(var_control)
			{
				case 0:
					switch(var_time.month)
					{
						case 1:
						case 3:
						case 5:
						case 7:
						case 8:
						case 10:
						case 12:
						if( var_time.day < 31 )
						{
							var_time.day++;
						}
						else
						{
							var_time.day = 1;
						}
						break;
						case 2:
						if( var_time.year % 4 )
						{
							if(var_time.day < 28)
							{
								var_time.day++;
							}
							else
							{
								var_time.day = 1;
							}
						}
						else
						{
							if(var_time.day < 29)
							{
								var_time.day++;
							}
							else
							{
								var_time.day = 1;
							}
						}
						break;
						case 4:
						case 6:
						case 9:
						case 11:
						if( var_time.day < 30)
						{
							var_time.day++;
						}
						else
						{
							var_time.day = 1;
						}
						break;
						default:
						break;
					}
				break;
				case 1:
				if( var_time.month < 12)
				{
					var_time.month++;
				}
				else
				{
					var_time.month = 1;
				}
				break;
				case 2:
				default:
				if( var_time.year < 2050)
				{
					var_time.year++;
				}
				else
				{
					var_time.year = 2010;
				}
				break;
			}
		}
		if(button.button.key.down == true)
		{
			switch(var_control)
			{
				case 0:
					switch(var_time.month)
					{
						case 1:
						case 3:
						case 5:
						case 7:
						case 8:
						case 10:
						case 12:
						if( var_time.day > 1)
						{
							var_time.day--;
						}
						else
						{
							var_time.day = 31;
						}
						break;
						case 2:
						if(var_time.year % 4)
						{
							if(var_time.day > 1)
							{
								var_time.day--;
							}
							else
							{
								var_time.day = 28;
							}
						}
						else
						{
							if(var_time.day > 1)
							{
								var_time.day--;
							}
							else
							{
								var_time.day = 29;
							}
						}
						break;
						case 4:
						case 6:
						case 9:
						case 11:
						if( var_time.day > 1)
						{
							var_time.day--;
						}
						else
						{
							var_time.day = 30;
						}
						break;
						default:
						break;
					}
				break;
				case 1:
				if( var_time.month > 1)
				{
					var_time.month--;
				}
				else
				{
					var_time.month = 12;
				}
				break;
				case 2:
				default:
				if( var_time.year > 2010)
				{
					var_time.year--;
				}
				else
				{
					var_time.year = 2050;
				}
				break;
			}
		}
		if(button.button.key.left == true)
		{
			if(var_control < 2)
			{
				var_control++;
			}
			else
			{
				var_control = 0;
			}
		}
		if(button.button.key.right == true)
		{
			if(var_control > 0)
			{
				var_control--;
			}
			else
			{
				var_control = 2;
			}
		}
		if(button.button.key.enter == true)
		{

#if APP_IHM_TASK_DEBUG
		PRINTF("\n\r[APPIHM]Config Date - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			rtc_datetime_t time = app_rtc_datetime_get();
			time.day = var_time.day;
			time.month = var_time.month;
			time.year = var_time.year;
			app_rtc_update(time);

			print = false;
			timeout_state = 0;
			ret = true;
			state = 0;
			*state_out = state_main_menu;
			return ret;
		}
		if(button.button.key.back == true)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Date - Back");
#endif /* APP_IHM_TASK_DEBUG */

			print = false;
			timeout_state = 0;
			ret = true;
			state = 0;
			*state_out = state_main_menu;
			return ret;
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Date - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					state = 0;
					*state_out = state_main_menu;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

	if(print == false)
	{
		print = true;
		/*1234567890123456*/
		/*  CONFIG. DATE  */
		/*   XX/XX/XXXX   */
		sprintf(data.line1,"  %s %s  ",display_txt_en[config],display_txt_en[date_]);
		sprintf(data.line2,"   %02d/%02d/%d   ",var_time.day,var_time.month,var_time.year);
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_auto_relay(data_ctrl_t button,uint8_t relay,st_ctrl_t *state_out)
{
	bool ret = false;
	static uint8_t timeout_state[APP_NUMBER_OF_RELAY] = {0};
	static uint32_t timeout[APP_NUMBER_OF_RELAY] = {0};
	static bool print[APP_NUMBER_OF_RELAY] = {false};
	static manager_relay_t config_rl[APP_NUMBER_OF_RELAY] = {{0},{0},{0}};
	static bool flag[APP_NUMBER_OF_RELAY] = {false};
	data_display_t data = {0,{0},{0}};

	if(flag[relay] == false)
	{
		flag[relay] = true;
		config_rl[relay] = app_manager_config_relay_get(relay);
	}
	if(button.flag_button == true)
	{
		/*  reset timeout */
		timeout_state[relay] = 0;

		/* limpa flag print */
		print[relay] = false;

		/* Checa Teclas */
		if((button.button.key.left == 1) || (button.button.key.right == 1))
		{
			if(config_rl[relay].var == variable_time)
			{
				config_rl[relay].var = variable_temp;
			}
			else
			{
				config_rl[relay].var = variable_time;
			}
		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print[relay] = false;
			flag[relay] = false;
			*state_out = state_config_auto;
			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay - Enter");
#endif /* APP_IHM_TASK_DEBUG */

		    app_manager_config_relay_set(relay,config_rl[relay]);
			ret = true;
			print[relay] = false;
			flag[relay] = false;

			if(config_rl[relay].var == variable_time)
			{
				switch(relay)
				{
					case 0:
					*state_out = state_config_auto_relay1_time;
					break;
					case 1:
					*state_out = state_config_auto_relay2_time;
					break;
					case 2:
					*state_out = state_config_auto_relay3_time;
					break;
					default:
					*state_out = state_config_auto;
					break;
				}
			}
			else
			{
				switch(relay)
				{
					case 0:
					*state_out = state_config_auto_relay1_temp;
					break;
					case 1:
					*state_out = state_config_auto_relay2_temp;
					break;
					case 2:
					*state_out = state_config_auto_relay3_temp;
					break;
					default:
					*state_out = state_config_auto;
					break;
				}
			}

			return ret;
		}
	}
	else
	{
		switch(timeout_state[relay])
		{
			case 0:
				timeout[relay] = app_ihm_tick_get();
				timeout_state[relay] = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout[relay]+APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay Time - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					ret = true;
					print[relay] = false;
					timeout_state[relay] = 0;
					*state_out = state_config_auto;
				}
			break;
			default:
				timeout_state[relay] = 0;
			break;
		}
	}


	if(print[relay] == false)
	{
		print[relay] = true;
		/*1234567890123456*/
		/*  CONFIG. RLn   */
		/*>>TIME   TEMPER.*/
		sprintf(data.line1,"  %s %s%d   ",display_txt_en[config],display_txt_en[RL],relay);
		sprintf(data.line2,"%s%s %s%s", config_rl[relay].var == variable_time ? ">>" : "  ",
										display_txt_en[time_],
										config_rl[relay].var == variable_time ? "  " : ">>",
										display_txt_en[temper]);
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}


	return ret;
}

/**
 *
 */
static bool app_ihm_config_auto_relay_time(data_ctrl_t button,uint8_t relay,st_ctrl_t *state_out)
{
	bool ret = false;
	static uint8_t timeout_state[APP_NUMBER_OF_RELAY] = {0};
	static uint32_t timeout[APP_NUMBER_OF_RELAY] = {0};
	static bool print[APP_NUMBER_OF_RELAY] = {false};
	static manager_relay_t config_rl[APP_NUMBER_OF_RELAY] = {{0},{0},{0}};
	static bool cursor[APP_NUMBER_OF_RELAY] = {false};
	static uint8_t state[APP_NUMBER_OF_RELAY] = {0};
	static bool flag[APP_NUMBER_OF_RELAY] = {false};
	data_display_t data = {0,{0},{0}};

	if(flag[relay] == false)
	{
		flag[relay] = true;
		config_rl[relay] = app_manager_config_relay_get(relay);
	}

	if(button.flag_button == true)
	{
		/*  reset timeout */
		timeout_state[relay] = 0;

		/* limpa flag print */
		print[relay] = false;

		/* Checa Teclas */
		if(button.button.key.up == 1)
		{
			switch(state[relay])
			{
				case 0: // hour_on | minute_on
				if(cursor[relay] == false)
				{
					if(config_rl[relay].hour_on < 23)
					{
						config_rl[relay].hour_on++;
					}
					else
					{
						config_rl[relay].hour_on = 0;
					}
				}
				else
				{
					if(config_rl[relay].minute_on < 60)
					{
						config_rl[relay].minute_on++;
					}
					else
					{
						config_rl[relay].minute_on = 0;
					}
				}
				break;
				case 1: // hour_off | minute
				if(cursor[relay] == false)
				{
					if(config_rl[relay].hour_off < 23)
					{
						config_rl[relay].hour_off++;
					}
					else
					{
						config_rl[relay].hour_off = 0;
					}
				}
				else
				{
					if(config_rl[relay].minute_off < 60)
					{
						config_rl[relay].minute_off++;
					}
					else
					{
						config_rl[relay].minute_off = 0;
					}
				}
				break;
			}
		}
		if(button.button.key.down == 1)
		{
			switch(state[relay])
			{
				case 0: // hour_on | minute_on
				if(cursor[relay] == false)
				{
					if(config_rl[relay].hour_on > 0)
					{
						config_rl[relay].hour_on--;
					}
					else
					{
						config_rl[relay].hour_on = 23;
					}
				}
				else
				{
					if(config_rl[relay].minute_on > 0)
					{
						config_rl[relay].minute_on--;
					}
					else
					{
						config_rl[relay].minute_on = 59;
					}
				}
				break;
				case 1: // hour_off | minute
				if(cursor[relay] == false)
				{
					if(config_rl[relay].hour_off > 0)
					{
						config_rl[relay].hour_off--;
					}
					else
					{
						config_rl[relay].hour_off = 23;
					}
				}
				else
				{
					if(config_rl[relay].minute_off > 0)
					{
						config_rl[relay].minute_off--;
					}
					else
					{
						config_rl[relay].minute_off = 59;
					}
				}
				break;
			}
		}
		if((button.button.key.right == 1) || (button.button.key.left == 1))
		{
			if(cursor[relay] == true)
			{
				cursor[relay] = false;
			}
			else
			{
				cursor[relay] = true;
			}
		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			cursor[relay] = false;
			state[relay] = 0;
			flag[relay] = false;
			print[relay] = false;
			*state_out = state_config_auto;
			return ret;
		}
		if(button.button.key.enter == 1)
		{
			print[relay] = false;
			//flag[relay] = false;
			cursor[relay] = false;
			//if(state[relay] < 2)
			//{
				state[relay]++;
			//}
			//else
			if(state[relay] >= 2)
			{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay - Enter");
#endif /* APP_IHM_TASK_DEBUG */

				ret = true;
				flag[relay] = false;
				state[relay] = 0;
				config_rl[relay].status = true;
				app_manager_config_relay_set(relay,config_rl[relay]);
				*state_out = state_config_auto_relay1;//state_config_auto;
				return ret;
			}
		}
	}
	else
	{
		switch(timeout_state[relay])
		{
			case 0:
				timeout[relay] = app_ihm_tick_get();
				timeout_state[relay] = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout[relay]+APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay Temp - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					ret = true;
					print[relay] = false;
					timeout_state[relay] = 0;
					*state_out = state_config_auto_relay1;//state_config_auto;
				}
			break;
			default:
				timeout_state[relay] = 0;
			break;
		}
	}

	switch(state[relay])
	{
		case 0:
			if(print[relay] == false)
			{
				print[relay] = true;
				/*1234567890123456*/
				/*  RLn TIME ON   */
				/*     XX:XX      */
				sprintf(data.line1,"  %s%d %s %s   ",display_txt_en[RL],relay,display_txt_en[time_],display_txt_en[on]);
				sprintf(data.line2,"     %02d:%02d      ",config_rl[relay].hour_on,config_rl[relay].minute_on);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 1:
			if(print[relay] == false)
			{
				print[relay] = true;
				/*1234567890123456*/
				/*  RLn TIME OFF  */
				/*     XX:XX      */
				sprintf(data.line1,"  %s%d %s %s  ",display_txt_en[RL],relay,display_txt_en[time_],display_txt_en[off]);
				sprintf(data.line2,"     %02d:%02d      ",config_rl[relay].hour_off,config_rl[relay].minute_off);
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_auto_relay_temperature(data_ctrl_t button,uint8_t relay,st_ctrl_t *state_out)
{
	bool ret = false;
	static uint8_t timeout_state[APP_NUMBER_OF_RELAY] = {0};
	static uint32_t timeout[APP_NUMBER_OF_RELAY] = {0};
	static bool print[APP_NUMBER_OF_RELAY] = {false};
	static manager_relay_t config_rl[APP_NUMBER_OF_RELAY] = {{0},{0},{0}};
//	static bool cursor[APP_NUMBER_OF_RELAY] = {false};
	static uint8_t state[APP_NUMBER_OF_RELAY] = {0};
	static bool flag[APP_NUMBER_OF_RELAY] = {false};
	data_display_t data = {0,{0},{0}};

	if(flag[relay] == false)
	{
		flag[relay] = true;
		config_rl[relay] = app_manager_config_relay_get(relay);
	}

	if(button.flag_button == true)
	{
		/*  reset timeout */
		timeout_state[relay] = 0;

		/* limpa flag print */
		print[relay] = false;

		/* Checa Teclas */
		if(button.button.key.up == 1)
		{
			if(state[relay] == 0)
			{
				if(config_rl[relay].temperature_on < 300)
				{
					config_rl[relay].temperature_on += 10;
				}
				else
				{
					config_rl[relay].temperature_on = 0;
				}
			}
			else
			{
				if(config_rl[relay].temperature_off < 300)
				{
					config_rl[relay].temperature_off += 10;
				}
				else
				{
					config_rl[relay].temperature_off = 0;
				}
			}
		}
		if(button.button.key.down == 1)
		{
			if(state[relay] == 0)
			{
				if(config_rl[relay].temperature_on > 0)
				{
					config_rl[relay].temperature_on -= 10;
				}
				else
				{
					config_rl[relay].temperature_on = 300;
				}
			}
			else
			{
				if(config_rl[relay].temperature_off > 0)
				{
					config_rl[relay].temperature_off -= 10;
				}
				else
				{
					config_rl[relay].temperature_off = 300;
				}
			}
		}
//		if((button.button.key.right == 1) || (button.button.key.left == 1))
//		{
//
//		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
//			cursor[relay] = false;
			state[relay] = 0;
			flag[relay] = false;
			print[relay] = false;
			*state_out = state_config_auto;
			return ret;
		}
		if(button.button.key.enter == 1)
		{
			print[relay] = false;
			//flag[relay] = false;
//			cursor[relay] = false;
			//if(state[relay] < 2)
			//{
				state[relay]++;
			//}
			//else
			if(state[relay] > 1)
			{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay - Enter");
#endif /* APP_IHM_TASK_DEBUG */

				ret = true;
				flag[relay] = false;
				state[relay] = 0;
				config_rl[relay].status = true;
				app_manager_config_relay_set(relay,config_rl[relay]);
				*state_out = state_config_auto_relay1;//state_config_auto;
				return ret;
			}
		}
	}
	else
	{
		switch(timeout_state[relay])
		{
			case 0:
				timeout[relay] = app_ihm_tick_get();
				timeout_state[relay] = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout[relay]+APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto Relay Temp - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					ret = true;
					print[relay] = false;
					timeout_state[relay] = 0;
					*state_out = state_config_auto_relay1;//state_config_auto;
				}
			break;
			default:
				timeout_state[relay] = 0;
			break;
		}
	}

	switch(state[relay])
	{
		case 0:
			if(print[relay] == false)
			{
				print[relay] = true;
				/*1234567890123456*/
				/* RLn TEMPER. ON */
				/*     XX.X C     */
				sprintf(data.line1," %s%d %s %s ",display_txt_en[RL],relay,display_txt_en[temper],display_txt_en[on]);
				sprintf(data.line2,"     %02d.%01d C     ",(config_rl[relay].temperature_on/10),(config_rl[relay].temperature_on%10));
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
		case 1:
			if(print[relay] == false)
			{
				print[relay] = true;
				/*1234567890123456*/
				/* RLn TEMPER. OFF*/
				/*     XX.X C     */
				sprintf(data.line1," %s%d %s %s",display_txt_en[RL],relay,display_txt_en[temper],display_txt_en[off]);
				sprintf(data.line2,"     %02d.%01d C     ",(config_rl[relay].temperature_off/10),(config_rl[relay].temperature_off%10));
				data.print = all;
				/* envia texto para */
				app_display_queue_send(data,0);
			}
		break;
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_auto_sd(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t cursor = 0;
	//static uint8_t state = 0;
	static bool print = false;
	//static bool flag = false;
	data_display_t data = {0,{0},{0}};
	bool ret = false;

	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

//		if(button.button.key.up == 1)
//		{
//
//		}
//		if(button.button.key.down == 1)
//		{
//
//		}
		if(button.button.key.left == 1)
		{
			if(cursor < 2)
			{
				cursor++;
			}
			else
			{
				cursor = 0;
			}
		}
		if(button.button.key.right == 1)
		{
			if(cursor > 0)
			{
				cursor--;
			}
			else
			{
				cursor = 2;
			}
		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto SD1 - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			*state_out = state_config_auto;
			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto SD1 - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			//state++;
			print = false;
			//if(state > 2)
			//{
				//state
				ret = true;
				switch(cursor)
				{
					case 0:
						*state_out = state_config_auto_sd_time_off;
					break;
					case 1:
						*state_out = state_config_auto_sd_time_on;
					break;
					case 2:
						*state_out = state_config_auto_sd_dc;
					break;
					default:
					break;
				}
				cursor = 0;
				return ret;
			//}
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Time - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					//state = 0;
					*state_out = state_config_auto;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

	if(print == false)
	{
		print = true;
		/*1234567890123456*/
		/*   CONFIG. SD1   */
		/*>>OFF >>ON  >>DC */
		sprintf(data.line1,"   %s %s  ",display_txt_en[config],display_txt_en[SD1]);
		sprintf(data.line2,"%s%s %s%s %s%s",cursor==0 ? ">>":"  ",display_txt_en[off],
											cursor==1 ? ">>":"  ",display_txt_en[on],
											cursor==2 ? ">>":"  ",display_txt_en[dc]);
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_auto_sd_time(data_ctrl_t button,uint8_t cursor,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t state = 0;
	static bool print = false;
	static bool flag = false;
//	static manager_sd_t data_sd;
	data_display_t data = {0,{0},{0}};
	bool ret = false;


    if(flag == false)
    {
    	flag = true;
    	data_sd = app_manager_config_sd_get();
    }
	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

		if(button.button.key.up == 1)
		{
			if(cursor == 0)
			{
				/* Time OFF */
				if(state == 0)
				{
					/* Hour OFF */
					if(data_sd.hour_off < 23)
					{
						data_sd.hour_off++;
					}
					else
					{
						data_sd.hour_off = 0;
					}
				}
				else
				{
					/* Minute OFF */
					if(data_sd.minute_off < 59)
					{
						data_sd.minute_off++;
					}
					else
					{
						data_sd.minute_off = 0;
					}
				}
			}
			else
			{
				/* Time ON */
				if(state == 0)
				{
					/* Hour ON */
					if(data_sd.hour_on < 23)
					{
						data_sd.hour_on++;
					}
					else
					{
						data_sd.hour_on = 0;
					}
				}
				else
				{
					/* Minute ON */
					if(data_sd.minute_on < 59)
					{
						data_sd.minute_on++;
					}
					else
					{
						data_sd.minute_on = 0;
					}
				}
			}
		}
		if(button.button.key.down == 1)
		{
			if(cursor == 0)
			{
				/* Time OFF */
				if(state == 0)
				{
					/* Hour OFF */
					if(data_sd.hour_off > 0)
					{
						data_sd.hour_off--;
					}
					else
					{
						data_sd.hour_off = 23;
					}
				}
				else
				{
					/* Minute OFF */
					if(data_sd.minute_off > 0)
					{
						data_sd.minute_off--;
					}
					else
					{
						data_sd.minute_off = 59;
					}
				}
			}
			else
			{
				/* Time ON */
				if(state == 0)
				{
					/* Hour ON */
					if(data_sd.hour_on > 0)
					{
						data_sd.hour_on--;
					}
					else
					{
						data_sd.hour_on = 23;
					}
				}
				else
				{
					/* Minute ON */
					if(data_sd.minute_on > 0)
					{
						data_sd.minute_on--;
					}
					else
					{
						data_sd.minute_on = 59;
					}
				}
			}
		}
//		if(button.button.key.left == 1)
//		{
//
//		}
//		if(button.button.key.right == 1)
//		{
//
//		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto SD1 - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			*state_out = state_config_auto_sd;

			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto SD1 - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			print = false;
			state++;
			if(state >= 2)
			{

				ret = true;
				state = 0;
				app_ihm_enable_sd_set(APP_CONFIGURED_TIME_OFF | APP_CONFIGURED_TIME_ON );
				if(app_ihm_enable_sd_get_status() == true)
				{
					data_sd.status = true;
				}
				app_manager_config_sd_set( data_sd );
				*state_out = state_config_auto_sd;
				return ret;
			}
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Time - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					//state = 0;
					*state_out = state_config_auto_sd;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

	if(print == false)
	{
		print = true;
		/*1234567890123456*/
		/*  SD1 TIME OFF  */
		/*     XX:XX      */
		sprintf(data.line1,"  %s %s %s  ",	display_txt_en[SD1],
											display_txt_en[time_],
											cursor==0? display_txt_en[off] : /*display_txt_en[on]*/"ON ");
		sprintf(data.line2,"     %02d:%02d      ",cursor==0? data_sd.hour_off : data_sd.hour_on,
												cursor==0? data_sd.minute_off : data_sd.minute_on);
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_config_auto_sd_dc(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	//static uint8_t state = 0;
	static bool print = false;
	static bool flag = false;
//	static manager_sd_t data_sd;
	data_display_t data = {0,{0},{0}};
	bool ret = false;

    if(flag == false)
    {
    	flag = true;
    	data_sd = app_manager_config_sd_get();
    }
	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

		if(button.button.key.up == 1)
		{
			if(data_sd.duty_cycle < 100)
			{
				data_sd.duty_cycle += 5;
			}
			else
			{
				data_sd.duty_cycle = 0;
			}
		}
		if(button.button.key.down == 1)
		{
			if(data_sd.duty_cycle > 0)
			{
				data_sd.duty_cycle -= 5;
			}
			else
			{
				data_sd.duty_cycle = 100;
			}
		}
//		if(button.button.key.left == 1)
//		{
//
//		}
//		if(button.button.key.right == 1)
//		{
//
//		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto SD1 DC - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			*state_out = state_config_auto_sd;

			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Auto SD1 DC - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			print = false;
			//state++;
			//if(state >= 2)
			//{

			ret = true;
			//state = 0;
			app_ihm_enable_sd_set(APP_CONFIGURED_TIME_SD);
			if(app_ihm_enable_sd_get_status() == true)
			{
				data_sd.status = true;
			}
			app_manager_config_sd_set( data_sd );
			*state_out = state_config_auto_sd;
			return ret;
			//}
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Config Time - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					//state = 0;
					*state_out = state_config_auto_sd;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

	if(print == false)
	{
		print = true;
		/*1234567890123456*/
		/* SD1 DUTY CYCLE */
		/*      XX %      */
		sprintf(data.line1," %s %s %s ",	display_txt_en[SD1],
											display_txt_en[duty],
											display_txt_en[cycle]);
		sprintf(data.line2,"      %02d %c      ", data_sd.duty_cycle, '%' );
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_view_telemetry_day(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t state = 0;
	static uint8_t day_index = 0;
	static bool print = false;
	static bool flag = false;
	//static manager_sd_t data_sd;
	data_display_t data = {0,{0},{0}};
	temperature_day_t temperature = {0};
	bool ret = false;

	if(flag == false)
	{
		flag = true;
		day_index = app_temperature_day_get();
	}
	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

		if(button.button.key.up == 1)
		{
			day_index++;
			if(day_index >= 7)
			{
				day_index = 0;
			}
			if(state < 6)
			{
				state++;
			}
			else
			{
				state = 0;
			}
		}
		if(button.button.key.down == 1)
		{
			if(day_index > 0)
			{
				day_index--;
			}
			else
			{
				day_index = 6;
			}
			if(state > 0)
			{
				state--;
			}
			else
			{
				state = 6;
			}
		}
//		if(button.button.key.left == 1)
//		{
//
//		}
//		if(button.button.key.right == 1)
//		{
//
//		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety View day - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			flag = false;
			*state_out = state_view_telemetry;
			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety View day - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			print = false;
			flag = false;
			ret = true;
			*state_out = state_view_telemetry;
			return ret;
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety View day - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					//state = 0;
					*state_out = state_view_telemetry;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

	if(print == false)
	{
		print = true;

		switch(state)
		{
			case 0:
				/*1234567890123456*/
				/*D MIN. AGE. MAX.*/
				/*n xx.x xx.x xx.x*/
				sprintf(data.line1,"D %s %s %s",display_txt_en[min],
												display_txt_en[avg],
												display_txt_en[max]);
				if(app_tempeature_telematic_day_get(day_index, &temperature) == true)
				{
					sprintf(data.line2,"%1d %02d.%1d %02d.%1d %02d.%1d",day_index,
							(temperature.temperature_min/10),(temperature.temperature_min%10),
							(temperature.temperature_avg/10),(temperature.temperature_avg%10),
							(temperature.temperature_max/10),(temperature.temperature_max%10));
				}
			break;
			default:
				/*1234567890123456*/
				/*n MIN. AGE. MAX.*/
				/*n xx.x xx.x xx.x*/
				/* Line 1 */
				if(app_tempeature_telematic_day_get(day_index, &temperature) == true)
				{
					sprintf(data.line1,"%1d %02d.%1d %02d.%1d %02d.%1d",(day_index + 1),
							(temperature.temperature_min/10),(temperature.temperature_min%10),
							(temperature.temperature_avg/10),(temperature.temperature_avg%10),
							(temperature.temperature_max/10),(temperature.temperature_max%10));
				}
				/* Line 2 */
				if((day_index + 1) >= 7)
				{
					if(app_tempeature_telematic_day_get(0 , &temperature) == true)
					{
						sprintf(data.line2,"%1d %02d.%1d %02d.%1d %02d.%1d",1,
								(temperature.temperature_min/10),(temperature.temperature_min%10),
								(temperature.temperature_avg/10),(temperature.temperature_avg%10),
								(temperature.temperature_max/10),(temperature.temperature_max%10));
					}
				}
				else
				{
					if(app_tempeature_telematic_day_get( (day_index + 1), &temperature ) == true)
					{
						sprintf(data.line2,"%1d %02d.%1d %02d.%1d %02d.%1d", (day_index + 2) ,
								(temperature.temperature_min/10),(temperature.temperature_min%10),
								(temperature.temperature_avg/10),(temperature.temperature_avg%10),
								(temperature.temperature_max/10),(temperature.temperature_max%10));
					}
				}
			break;
		}
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_view_telemetry(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t cursor = 0;
	//static uint8_t hour_index = 0;
	static bool print = false;
	//static bool flag = false;
	data_display_t data = {0,{0},{0}};
	bool ret = false;

	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

//		if(button.button.key.up == 1)
//		{
//		}
//		if(button.button.key.down == 1)
//		{
//		}
		if((button.button.key.left == 1) || (button.button.key.right == 1))
		{
			if(cursor == 0)
			{
				cursor = 1;
			}
			else
			{
				cursor = 0;
			}
		}
//		if(button.button.key.right == 1)
//		{
//
//		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety Menu hour - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			//flag = false;
			*state_out = state_main_menu;
			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety Menu hour - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			print = false;
			//flag = false;
			ret = true;
			if(cursor==0)
			{
				*state_out = state_view_telemetry_day;
			}
			else
			{
				*state_out = state_view_telemetry_hour;
			}
			return ret;
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety Menu hour - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					//state = 0;
					*state_out = state_main_menu;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

	if(print == false)
	{
		print = true;
		/*1234567890123456*/
		/* VIEW TELEMETRY */
		/* >>DAY   >>HOUR */
		sprintf(data.line1," %s %s ",display_txt_en[view],
									 display_txt_en[telemety]);
		sprintf(data.line2," %s%s   %s%s ", cursor==0?">>":"  ",
											display_txt_en[day],
											cursor==0?"  ":">>",
											display_txt_en[hour]);
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static bool app_ihm_view_telemetry_hour(data_ctrl_t button,st_ctrl_t *state_out)
{
	static uint8_t timeout_state = 0;
	static uint32_t timeout = 0;
	static uint8_t state = 0;
	static uint8_t hour_index_line1 = 0;
	static uint8_t hour_index_line2 = 0;
	static bool print = false;
	static bool flag = false;
	int16_t temperature_hour[2] = {0};
	data_display_t data = {0,{0},{0}};
	bool ret = false;


	if(flag == false)
	{
		flag = true;
		hour_index_line1 = app_temperature_hour_get();
		if(hour_index_line1 == 22)
		{
			hour_index_line2 = 1;
		}
		else if(hour_index_line1 >= 23)
		{
			hour_index_line2 = 2;
		}
		else
		{
			hour_index_line2 = hour_index_line2 + 3;
		}
	}
	if(button.flag_button == true)
	{
		/* Reset timeout */
		timeout_state = 0;
		/* clear flag print */
		print = false;

		if(button.button.key.up == 1)
		{
			if(hour_index_line1 < 23)
			{
				hour_index_line1++;
			}
			else
			{
				hour_index_line1 = 0;
			}
			if(hour_index_line2 < 23)
			{
				hour_index_line2++;
			}
			else
			{
				hour_index_line2 = 0;
			}
			if(state < 11)
			{
				state++;
			}
			else
			{
				state = 0;
			}
		}
		if(button.button.key.down == 1)
		{
			if(hour_index_line1 > 0)
			{
				hour_index_line1--;
			}
			else
			{
				hour_index_line1 = 23;
			}
			if(hour_index_line2 > 0)
			{
				hour_index_line2--;
			}
			else
			{
				hour_index_line2 = 23;
			}
			if(state > 0)
			{
				state--;
			}
			else
			{
				state = 11;
			}
		}
//		if(button.button.key.left == 1)
//		{
//
//		}
//		if(button.button.key.right == 1)
//		{
//
//		}
		if(button.button.key.back == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety View hour - Back");
#endif /* APP_IHM_TASK_DEBUG */

			ret = true;
			print = false;
			flag = false;
			*state_out = state_view_telemetry;
			return ret;
		}
		if(button.button.key.enter == 1)
		{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety View hour - Enter");
#endif /* APP_IHM_TASK_DEBUG */

			print = false;
			flag = false;
			ret = true;
			*state_out = state_view_telemetry;
			return ret;
		}
	}
	else
	{
		switch(timeout_state)
		{
			case 0:
				timeout = app_ihm_tick_get();
				timeout_state = 1;
			break;
			case 1:
				if(app_ihm_tick_get() > (timeout + APP_TIMEOUT))
				{

#if APP_IHM_TASK_DEBUG
	PRINTF("\n\r[APPIHM]Telemety View hour - Timeout");
#endif /* APP_IHM_TASK_DEBUG */

					print = false;
					timeout_state = 0;
					ret = true;
					flag = false;
					//state = 0;
					*state_out = state_view_telemetry;
					return ret;
				}
			break;
			default:
				timeout_state = 0;
			break;
		}
	}

	if(print == false)
	{

		print = true;
		switch(state)
		{
			case 0:
				/*1234567890123456*/
				/*TEMPERATURE HOUR*/
				/*HH XX.X  HH XX.X*/
				sprintf(data.line1,"%s %s",display_txt_en[tempe],
											display_txt_en[hour]);

				if(hour_index_line1 < 23)
				{
					app_temperature_telematic_hour_get(hour_index_line1,&temperature_hour[0]);
					app_temperature_telematic_hour_get((hour_index_line1+1),&temperature_hour[1]);
					sprintf(data.line2,"%02d %02d.%1d  %02d %02d.%1d", hour_index_line1,
							(temperature_hour[0]/10), (temperature_hour[0]%10),
							(hour_index_line1+1),
							(temperature_hour[1]/10), (temperature_hour[1]%10) );
				}
				else
				{
					app_temperature_telematic_hour_get(hour_index_line1,&temperature_hour[0]);
					app_temperature_telematic_hour_get( 0 ,&temperature_hour[1]);
					sprintf(data.line2,"%02d %02d.%1d  %02d %02d.%1d", hour_index_line1,
							(temperature_hour[0]/10), (temperature_hour[0]%10),
							0,
							(temperature_hour[1]/10), (temperature_hour[1]%10) );
				}
			break;
			case 1:
			default:
				/* Line 1 */
				if(hour_index_line1 < 23)
				{
					app_temperature_telematic_hour_get(hour_index_line1,&temperature_hour[0]);
					app_temperature_telematic_hour_get((hour_index_line1+1),&temperature_hour[1]);
					sprintf(data.line1,"%02d %02d.%1d  %02d %02d.%1d", hour_index_line1,
							(temperature_hour[0]/10), (temperature_hour[0]%10),
							(hour_index_line1+1),
							(temperature_hour[1]/10), (temperature_hour[1]%10) );
				}
				else
				{
					app_temperature_telematic_hour_get(hour_index_line1,&temperature_hour[0]);
					app_temperature_telematic_hour_get( 0 ,&temperature_hour[1]);
					sprintf(data.line1,"%02d %02d.%1d  %02d %02d.%1d", hour_index_line1,
							(temperature_hour[0]/10), (temperature_hour[0]%10),
							0,
							(temperature_hour[1]/10), (temperature_hour[1]%10) );
				}
				/* Line 2 */
				if((hour_index_line2+1) < 23)
				{
					app_temperature_telematic_hour_get((hour_index_line2+1),&temperature_hour[0]);
					app_temperature_telematic_hour_get((hour_index_line2+2),&temperature_hour[1]);
					sprintf(data.line2,"%02d %02d.%1d  %02d %02d.%1d", hour_index_line2,
							(temperature_hour[0]/10), (temperature_hour[0]%10),
							(hour_index_line2+1),
							(temperature_hour[1]/10), (temperature_hour[1]%10) );
				}
				else
				{
					app_temperature_telematic_hour_get((hour_index_line2+1),&temperature_hour[0]);
					app_temperature_telematic_hour_get( 0 ,&temperature_hour[1]);
					sprintf(data.line2,"%02d %02d.%1d  %02d %02d.%1d", hour_index_line2,
							(temperature_hour[0]/10), (temperature_hour[0]%10),
							0,
							(temperature_hour[1]/10), (temperature_hour[1]%10) );
				}
			break;
		}
		data.print = all;
		/* envia texto para */
		app_display_queue_send(data,0);
	}

	return ret;
}

/**
 *
 */
static void app_ihm_enable_sd_set(uint8_t param)
{
	enable_sd |= param;
}

/**
 *
 */
static bool app_ihm_enable_sd_get_status(void)
{
	if(enable_sd == (APP_CONFIGURED_TIME_OFF | APP_CONFIGURED_TIME_ON | APP_CONFIGURED_TIME_SD))
	{
		return true;
	}
	else
	{
		return false;
	}
}
