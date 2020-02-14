/*
 * AppManager.c
 *
 *  Created on: 13/10/2019
 *      Author: evandro
 */

#include "AppControll.h"

/**
 *
 */
#define RELAY_SET(index, st)				\
{											\
	do										\
	{										\
		switch(index)						\
		{									\
			case 0:							\
				app_io_relay1_set(st);		\
			break;							\
			case 1:							\
				app_io_relay2_set(st);		\
			break;							\
			case 2:							\
				app_io_relay3_set(st);		\
			break;							\
		}									\
	}while(0);								\
}


/**
 *
 */
manager_relay_t relay_app[APP_NUMBER_OF_RELAY] = {{0},{0},{0}};
//io_status_t relay_status[APP_NUMBER_OF_RELAY] = {0};
//io_status_t status_relay[APP_NUMBER_OF_RELAY] = {0};
manager_sd_t sd_app = {0};
state_sd_t state_sd = sd_off;

/**
 *
 */
static void app_controll_task(void *pvParameters);

/**
 *
 */
void app_controll_init_task(void)
{
	if(xTaskCreate(app_controll_task,"Controll_task",APP_CONTROLL_TASK_STATCK, NULL, APP_PRIORITIES_MED, NULL) != pdPASS)
	{
#if APP_CONTROLL_TASK_DEBUG
		PRINTF("\n\r[APPCONTROL]Manager_task Create\n\r");
#endif /* APP_MANAGER_TASK_DEBUG */

	}
}

/**
 *
 */
static void app_controll_task(void *pvParameters)
{
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL]Manager_task");
	bool debug_controll = false;
#endif /* APP_MANAGER_TASK_DEBUG */

	rtc_datetime_t data_rtc = {0};
	uint8_t minute_old = 0;
//	state_sd_t state_sd = sd_off;
	pwm_t pwm_sd = {0};
	uint8_t counter_sd = 0;
	uint8_t duty_cycle_sd = 0;
	uint8_t index_relay;
	uint8_t state_relay_temp[APP_NUMBER_OF_RELAY] = {0};
	io_status_t status_relay[APP_NUMBER_OF_RELAY] = {0};

	while(1)
	{
		data_rtc = app_rtc_datetime_get();

		if(minute_old != data_rtc.minute)
		{
			// Controle dos reles
			for(index_relay = 0;index_relay < APP_NUMBER_OF_RELAY;index_relay++)
			{
				// Checa se relay esta habilitado
				if(app_manager_config_relay_get(index_relay).status == true)
				{
					if(app_manager_config_relay_get(index_relay).var == variable_time)
					{
						if( data_rtc.hour >= app_manager_config_relay_get(index_relay).hour_off )
						{
							if( data_rtc.minute >= app_manager_config_relay_get(index_relay).minute_off )
							{
								if(status_relay[index_relay] == io_on)
								{
									status_relay[index_relay] = io_off;
									RELAY_SET(index_relay,io_off);
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("Relay %d ON",(index_relay+1));
#endif /* APP_CONTROLL_TASK_DEBUG */
								}
							}
						}
						else if (( data_rtc.hour >= app_manager_config_relay_get(index_relay).hour_on ) &&
								 ( data_rtc.hour < app_manager_config_relay_get(index_relay).hour_off ))
						{
							if( data_rtc.minute >= app_manager_config_relay_get(index_relay).minute_on )
							{
								//if((status_relay[index_relay] == io_off) || (status_relay[index_relay] == io_idle))
								if(status_relay[index_relay] == io_off)
								{
									status_relay[index_relay] = io_on;
									RELAY_SET(index_relay,io_on);
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("Relay %d OFF",(index_relay+1));
#endif /* APP_CONTROLL_TASK_DEBUG */
								}
							}
						}
//						else
//						{
//							if((data_rtc.hour == 0)&&(data_rtc.minute == 0))
//							{
//#if APP_CONTROLL_TASK_DEBUG
//	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
//	PRINTF("OUT Relay %d Ilde",(index_relay+1));
//#endif /* APP_CONTROLL_TASK_DEBUG */
//							}
//							else
//							{
//								if( ( data_rtc.hour >= app_manager_config_relay_get(index_relay).hour_on ) &&
//									( data_rtc.hour < app_manager_config_relay_get(index_relay).hour_off ) )
//								{
//									if(data_rtc.minute >= app_manager_config_relay_get(index_relay).minute_on)
//									{
//#if APP_CONTROLL_TASK_DEBUG
//	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
//	PRINTF("OUT Relay %d Ilde",(index_relay+1));
//#endif /* APP_CONTROLL_TASK_DEBUG */
//									}
//								}
//							}
//						}
					}
					else
					{
						switch(state_relay_temp[index_relay])
						{
							case 0:
								if(app_temperature_get() <= app_manager_config_relay_get(index_relay).temperature_on)
								{
									if(status_relay[index_relay] == io_off)
									{
										status_relay[index_relay] = io_on;
										RELAY_SET(index_relay,io_on);
										state_relay_temp[index_relay] = 1;
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("\n\r[APPCONTROLL] Relay %d ON",(index_relay+1));
	PRINTF("\n\r[APPCONTROLL] Temperature %02d.%02d ",(app_manager_config_relay_get(index_relay).temperature_on / 10),
													  (app_manager_config_relay_get(index_relay).temperature_on % 10));
#endif /* APP_CONTROLL_TASK_DEBUG */
									}
								}
								//if(*status_relay[index_relay] == io_on)
								//{
								//	state_relay_temp[index_relay] = 1;
								//}
							break;
							case 1:
							default:
								if(app_temperature_get() > app_manager_config_relay_get(index_relay).temperature_off)
								{
									if(status_relay[index_relay] == io_on)
									{
										status_relay[index_relay] = io_off;
										RELAY_SET(index_relay,io_off);
										state_relay_temp[index_relay] = 0;
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("\n\r[APPCONTROLL] Relay %d OFF",(index_relay+1));
	PRINTF("\n\r[APPCONTROLL] Temperature %02d.%02d ",(app_manager_config_relay_get(index_relay).temperature_on / 10),
													  (app_manager_config_relay_get(index_relay).temperature_on % 10));
#endif /* APP_CONTROLL_TASK_DEBUG */
									}
								}
							break;
						}
					}
				}
			}

			// Controle da Saida Digital
			if(app_manager_config_sd_get().status == true)
			{
				switch(state_sd)
				{
					case sd_off:
#if APP_CONTROLL_TASK_DEBUG
	if(debug_controll == false)
	{
		debug_controll = true;
		PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
		PRINTF("IN Off state ");
	}
#endif /* APP_CONTROLL_TASK_DEBUG */
						if(data_rtc.hour >= app_manager_config_sd_get().hour_on)
						{
							if(data_rtc.minute >= app_manager_config_sd_get().minute_on)
							{
								// Set Saida Digital
								state_sd = sd_turning_on;
								pwm_sd.status = true;
								pwm_sd.duty_cycle = 1;
								duty_cycle_sd = 1;
								counter_sd = 0;
								app_pwm_set(pwm_sd);
#if APP_CONTROLL_TASK_DEBUG
	debug_controll = false;
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("OUT Off state ");
#endif /* APP_CONTROLL_TASK_DEBUG */
							}
						}
					break;
					case sd_turning_on:
						if(counter_sd < APP_TIME_SD_AUTO)
						{
							counter_sd++;
						}
						else
						{
							counter_sd = 0;
							if(duty_cycle_sd < app_manager_config_sd_get().duty_cycle)
							{
								duty_cycle_sd++;
								pwm_sd.duty_cycle = duty_cycle_sd;
								app_pwm_set(pwm_sd);
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("Turning On State DC: %d ",duty_cycle_sd);
#endif /* APP_CONTROLL_TASK_DEBUG */
							}
							else
							{
								state_sd = sd_on;
#if APP_CONTROLL_TASK_DEBUG
	debug_controll = false;
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("OUT Turning On State DC: %d ",duty_cycle_sd);
#endif /* APP_CONTROLL_TASK_DEBUG */
							}
						}
					break;
					case sd_on:
#if APP_CONTROLL_TASK_DEBUG
	if(debug_controll == false)
	{
		debug_controll = true;
		PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
		PRINTF("IN On State ");
	}
#endif /* APP_CONTROLL_TASK_DEBUG */
						if(data_rtc.hour >= app_manager_config_sd_get().hour_off)
						{
							if(data_rtc.hour == app_manager_config_sd_get().hour_off)
							{
								if(data_rtc.minute >= app_manager_config_sd_get().minute_off)
								{
									duty_cycle_sd = app_manager_config_sd_get().duty_cycle;
									state_sd = sd_turning_off;
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("OUT Turning off start DC: %d ",duty_cycle_sd);
#endif /* APP_CONTROLL_TASK_DEBUG */
								}
							}
							else
							{
								duty_cycle_sd = app_manager_config_sd_get().duty_cycle;
								state_sd = sd_turning_off;
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("OUT Turning off start DC: %d ",duty_cycle_sd);
#endif /* APP_CONTROLL_TASK_DEBUG */
							}
						}
					break;
					case sd_turning_off:
						if(counter_sd < APP_TIME_SD_AUTO)
						{
							counter_sd++;
						}
						else
						{
							counter_sd = 0;
							if(duty_cycle_sd > 0)
							{
								duty_cycle_sd--;
								pwm_sd.duty_cycle = duty_cycle_sd;
								app_pwm_set(pwm_sd);
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("Turning off start DC: %d ",duty_cycle_sd);
#endif /* APP_CONTROLL_TASK_DEBUG */
							}
							else
							{
								pwm_sd.status = false;
								state_sd = sd_idle;
								app_pwm_set(pwm_sd);
#if APP_CONTROLL_TASK_DEBUG
	debug_controll = false;
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("OUT Turning off start DC: %d ",duty_cycle_sd);
#endif /* APP_CONTROLL_TASK_DEBUG */
							}
						}
					break;
					case sd_idle:
#if APP_CONTROLL_TASK_DEBUG
	if(debug_controll == false)
	{
		debug_controll = true;
		PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
		PRINTF("IN Idle State ");
	}
#endif /* APP_CONTROLL_TASK_DEBUG */
						if((data_rtc.hour == 0)&&(data_rtc.minute == 0))
						{
							state_sd = sd_off;
#if APP_CONTROLL_TASK_DEBUG
	debug_controll = false;
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("OUT Idle State ");
#endif /* APP_CONTROLL_TASK_DEBUG */
						}
						else
						{
							if( (data_rtc.hour >= app_manager_config_sd_get().hour_on) &&
								(data_rtc.hour < app_manager_config_sd_get().hour_off) )
							{
								if(data_rtc.minute >= app_manager_config_sd_get().minute_on)
								{
									// Set Saida Digital
									state_sd = sd_turning_on;
									pwm_sd.status = true;
									pwm_sd.duty_cycle = 1;
									duty_cycle_sd = 1;
//									counter_sd = 0;
									app_pwm_set(pwm_sd);
#if APP_CONTROLL_TASK_DEBUG
	debug_controll = false;
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("OUT Idle State ");
#endif /* APP_CONTROLL_TASK_DEBUG */
								}
							}
						}
					break;
					default:
						counter_sd = 0;
						duty_cycle_sd = 0;
						state_sd = sd_off;
						pwm_sd.duty_cycle = 0;
						pwm_sd.status = false;
						app_pwm_set(pwm_sd);
#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] %02d:%02d ",data_rtc.hour,data_rtc.minute);
	PRINTF("Internal ");
#endif /* APP_CONTROLL_TASK_DEBUG */
					break;
				}
			}

			// Update minute
			minute_old = data_rtc.minute;
		}
//		else
//		{
//			// Update minute
//			//minute_old = data_rtc.minute;
//		}

//		// Update minute
//		minute_old = data_rtc.minute;
		vTaskDelay(1000); // 01 segundo
	}
}

/**
 *
 */
void app_manager_config_relay_set(uint8_t relay, manager_relay_t param)
{
	relay_app[relay] = param;
}

/**
 *
 */
manager_relay_t app_manager_config_relay_get(uint8_t relay)
{
	return relay_app[relay];
}

/**
 *
 */
void app_manager_config_sd_set(manager_sd_t param)
{
	sd_app = param;

#if APP_CONTROLL_TASK_DEBUG
	PRINTF("\n\r[APPCONTROLL] Set Param: ");
	PRINTF("\n\r[APPCONTROLL] Duty Cycle: %d ",param.duty_cycle);
	PRINTF("\n\r[APPCONTROLL] ON: %02d:%02d ",param.hour_on,param.minute_on);
	PRINTF("\n\r[APPCONTROLL] OFF: %02d:%02d ",param.hour_off,param.minute_off);
#endif /* APP_CONTROLL_TASK_DEBUG */
}

/**
 *
 */
manager_sd_t app_manager_config_sd_get(void)
{
	return sd_app;
}

/**
 *
 */
state_sd_t app_manager_state_sd_get(void)
{
	return state_sd;
}

/**
 *
 */
//io_status_t app_manager_status_relay_get(uint8_t relay)
//{
//	return status_relay[relay];
//}
