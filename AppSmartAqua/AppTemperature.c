/*
 * AppTemperature.c
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */
#include "AppTemperature.h"
#include <math.h>
/**
 *
 */
#define APP_TEMP_DAY	7
#define APP_HOUR_DAY	24
/**
 *
 */
static void app_tempeature_telematic(void);
//static int16_t app_temperature_get(void);
static void app_temperature_set(int16_t temp);
static void app_temperature_adc_init(void);
static void app_temperature_adc_start(void);
static void app_temperature_irq(void);
static int16_t app_temperature_calculate(uint16_t *buffer);
static void app_temperature_init(void);
static void app_temperature_task(void *pvParameters);
//static void app_temperature_queue_init(void);
/**
 *
 */
static adc_channel_config_t adcChannelConfigStruct;
//static uint16_t buffer_adc[APP_TEMPERATURE_SIZE_BUFFER] = {0};
//static uint8_t index_adc = 0;
static QueueHandle_t adc_queue = NULL;
static temperature_day_t temperature_day[APP_TEMP_DAY];// = {0,0,0};
static int16_t temperature = 0;
static uint8_t index_day = 0;
static uint8_t index_hour = 0;
static int16_t temperature_hour[APP_HOUR_DAY] = {0};

/**
 *
 */
void app_temperature_init_task(void)
{
	if(xTaskCreate(app_temperature_task,"Temp_Task",APP_TEMP_TASK_STATCK, NULL, APP_PRIORITIES_LOW, NULL) != pdPASS)
	{
#if APP_TEMP_TASK_DEBUG
		PRINTF("\n\r[APPTemp]Temp_task Fail \n\r");
#endif /* APP_TEMP_TASK_DEBUG */
	}

	//app_temperature_queue_init();
}

/**
 *
 */
void app_temperature_queue_create(void)
{
	adc_queue = xQueueCreate(APP_ADC_MAX_LENGTH,sizeof(uint16_t) );
	if(adc_queue)
	{
#if APP_TEMP_TASK_DEBUG
		PRINTF("\n\r[APPTemp]Create Queue ADC ");
#endif /* APP_TEMP_TASK_DEBUG */
	}
}

/**
 *
 */
static void app_temperature_task(void *pvParameters)
{
	static uint16_t buffer_adc[APP_TEMPERATURE_SIZE_BUFFER] = {0};
	static uint8_t index_adc = 0;
	uint16_t data;

#if APP_TEMP_TASK_DEBUG
		PRINTF("\n\r[APPTemp]Create temperature task");
#endif /* APP_TEMP_TASK_DEBUG */

	app_temperature_init();

	while(1)
	{
		if( xQueueReceive(adc_queue,&data,portMAX_DELAY) == pdTRUE )
		{

#if APP_TEMP_TASK_DEBUG
		//PRINTF("\n\r[APPTemp]ADC: %d",data);
#endif /* APP_TEMP_TASK_DEBUG */

			buffer_adc[index_adc++] = data;
			if(index_adc >= APP_TEMPERATURE_SIZE_BUFFER)
			{
				index_adc = 0;
			}

			app_temperature_set( app_temperature_calculate(buffer_adc) );
		}

		app_tempeature_telematic();
	}
}

/**
 *
 */
static void app_temperature_set(int16_t temp)
{
	temperature = temp;
}

/**
 *
 */
int16_t app_temperature_get(void)
{
	return temperature;
}

/**
 *
 */
uint8_t app_temperature_day_get(void)
{
	return index_day;
}

/**
 *
 */
uint8_t app_temperature_hour_get(void)
{
	return index_hour;
}

/**
 *
 */
bool app_temperature_telematic_hour_get(uint8_t hour, int16_t *temp)
{
	if(hour < APP_HOUR_DAY)
	{
		*temp = temperature_hour[hour];
		return true;
	}
	else
	{
		return false;
	}
}

/**
 *
 */
bool app_tempeature_telematic_day_get(uint8_t day,temperature_day_t *temp)
{
	if(day < APP_TEMP_DAY)
	{
		*temp = temperature_day[day];
		return true;
	}
	else
	{
		return false;
	}
}

/**
 *
 */
static void app_tempeature_telematic(void)
{
	static int32_t temp_avg = 0;
	static uint8_t state = 0;
	static uint8_t hour_old = 0;
	int16_t temp = app_temperature_get();

	switch(state)
	{
		case 0:
			if((app_rtc_datetime_get().hour == 0) && (app_rtc_datetime_get().minute == 0))
			{
				temperature_day[index_day].temperature_max = temp;
				temperature_day[index_day].temperature_min = temp;
				temperature_day[index_day].temperature_avg = temp;
				temp_avg = temp;
				hour_old = app_rtc_datetime_get().hour;
				temperature_hour[index_hour++] = temp;
				state = 1;
				if(index_hour > APP_HOUR_DAY)
				{
					index_hour = 0;
				}
			}
		break;
		case 1:
		default:
			if((app_rtc_datetime_get().minute == 0) && (hour_old != app_rtc_datetime_get().hour))
			{

				if(app_rtc_datetime_get().hour != 0)
				{
					if(temperature_day[index_day].temperature_max < temp)
					{
						temperature_day[index_day].temperature_max = temp;
					}
					if(temperature_day[index_day].temperature_min > temp)
					{
						temperature_day[index_day].temperature_min = temp;
					}
					temperature_hour[index_hour++] = temp;
					temp_avg += temp;
					hour_old = app_rtc_datetime_get().hour;
					if(index_hour > APP_HOUR_DAY)
					{
						index_hour = 0;
					}
				}
				else
				{
					temperature_day[index_day++].temperature_avg = (int16_t)((float)temp_avg / APP_HOUR_DAY);
					if(index_day >= APP_TEMP_DAY)
					{
						index_day = 0;
					}
					state = 0;
				}
			}
		break;
	}
}

/**
 *
 */
static void app_temperature_init(void)
{
#if APP_TEMPERATURE_ANALOG

	app_temperature_adc_init();
	app_temperature_adc_start();
#if APP_TEMP_TASK_DEBUG
		PRINTF("\n\r[APPTemp]Init Sensor Analog");
#endif /* APP_TEMP_TASK_DEBUG */

#else
// 	APP_TEMPERATURE_DIGITAL

#if APP_TEMP_TASK_DEBUG
		PRINTF("\n\r[APPTemp]Init Sensor Digital");
#endif /* APP_TEMP_TASK_DEBUG */

#endif

	app_timer_pit_init(125/* 125 ms*/,1);
	app_timer_pit_callback(1,app_temperature_irq);
	app_timer_pit_start(1);

#if APP_TEMP_TASK_DEBUG
		PRINTF("\n\r[APPTemp]Init Timer 125 ms");
#endif /* APP_TEMP_TASK_DEBUG */

}

/**
 *
 */
static void app_temperature_adc_init(void)
{
    adc_config_t adcConfigStrcut;

    EnableIRQ(ADC_IRQn);

    /*
     * config->referenceVoltageSource = kADC_ReferenceVoltageSourceAlt0;
     * config->enableLowPower = false;
     * config->enableLongSampleTime = false;
     * config->clockDivider = kADC_ClockDivider1;
     * config->ResolutionMode = kADC_Resolution8BitMode;
     * config->clockSource = kADC_ClockSourceAlt0;
     */
    ADC_GetDefaultConfig(&adcConfigStrcut);
    adcConfigStrcut.ResolutionMode = kADC_Resolution12BitMode;
    ADC_Init(APP_TEMPERATURE_BASE, &adcConfigStrcut);
    ADC_EnableHardwareTrigger(APP_TEMPERATURE_BASE, false);

    /* Configure the user channel and interrupt. */
    adcChannelConfigStruct.channelNumber = APP_TEMPERATURE_CHANNEL;
    adcChannelConfigStruct.enableInterruptOnConversionCompleted = true;
    adcChannelConfigStruct.enableContinuousConversion = false;

    /* Enable the releated analog pins. */
    ADC_EnableAnalogInput(APP_TEMPERATURE_BASE, 1U << APP_TEMPERATURE_CHANNEL, true);
}

/**
 *
 */
static int16_t app_temperature_calculate(uint16_t *buffer)
{
	const uint16_t resistor_r1 = 1000; 		/* Resistor 1K 	*/
	const uint16_t voltage = 5000; 			/* VCC = 5V 	*/
	const float const_a = 0.0011303F; 		/* Const A 		*/
	const float const_b = 0.0002339F; 		/* Const B 		*/
	const float const_c = 0.00000008863F;	/* Const C 		*/
	float var_b = 0.00F;
	float var_c = 0.00F;
	float temp = 0.00F;
	int16_t temperature;
	uint16_t Vin = 0;
	uint16_t rNTC = 0;
	uint8_t i = 0;
	uint32_t avg_adc = 0;

	/* ADC summation */
	for(i=0;i<APP_TEMPERATURE_SIZE_BUFFER;i++)
	{
		avg_adc += buffer[i];
	}

	/* Calculates ADC Average */
	avg_adc = (uint32_t)((float)avg_adc / APP_TEMPERATURE_SIZE_BUFFER);

	/* Vin = (VCC * adc) / 16Bit */
	Vin = (uint16_t)((float)(0x0FFF * avg_adc) / voltage );

	/* Resistor NTC = ((VCC * R1) / Vin) - R1 */
	rNTC = (uint16_t)((float) (((voltage * resistor_r1) / Vin) - resistor_r1) );

   /* The relationship between resistance and temperature in the NTC is given by the Steinhart & Hart equation.
	* Temp = 1 / ( a + b x ln(rNTC) + c x ln³(rNTC) ) */
    var_b = (float)( log(rNTC) );
	var_c = (float)( pow(var_b,3) );
	var_c = (float)( const_c * var_c );
	var_b = (float)( const_b * var_b );

	temp  = (float)( const_a + var_b + var_c );
	temp  = (float)( 1.00F / temp );
	temp  = (float)( temp - 273.15F );
	temperature = (int16_t)( temp * 10.00F );

	return (int16_t)(temperature);
}

/**
 *
 */
static void app_temperature_irq(void)
{
	app_temperature_adc_start();
}

/**
 *
 */
static void app_temperature_adc_start(void)
{
	ADC_SetChannelConfig(APP_TEMPERATURE_BASE, &adcChannelConfigStruct);
}

/**
 *
 */
void ADC_IRQHandler(void)
{
//	static uint8_t index_adc = 0;
//	buffer_adc[index_adc++] = ADC_GetChannelConversionValue(APP_TEMPERATURE_BASE);
//	if(index_adc >= APP_TEMPERATURE_SIZE_BUFFER)
//	{
//		index_adc = 0;
//	}

	uint16_t data = (uint16_t)(ADC_GetChannelConversionValue(APP_TEMPERATURE_BASE));
    xQueueSendToFrontFromISR(adc_queue,&data,pdFALSE);
}
