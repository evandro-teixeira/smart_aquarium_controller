/*
 * AppPwm.c
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */
#include "AppPwm.h"

static pwm_t param_pwm;

#if APP_TYPE_PWM
#else
static uint32_t count_pwm = 0;
static uint32_t time_pwm = 0;
static void app_pwm_irq(void);
#endif /* APP_TYPE_PWM */
/**
 *
 */
void app_pwm_init(void)
{
	// PTD1_PWM3
#if APP_TYPE_PWM
	PORT_SetPinSelect(kPORT_FTM2CH3, kPORT_FTM2_CH3_PTD1);     /* pin  is configured as  */

    ftm_config_t ftmInfo;
    ftm_chnl_pwm_signal_param_t ftmParam;
    ftm_pwm_level_select_t pwmLevel = kFTM_LowTrue;

    //app_sd_duty_cycle = 2; // 50 %
    param_pwm.duty_cycle = 20;
    param_pwm.status = true;

    /* Configure ftm params with frequency 24kHZ */
    ftmParam.chnlNumber = APP_FTM_CHANNEL;
    ftmParam.level = pwmLevel;
    ftmParam.dutyCyclePercent = param_pwm.duty_cycle;
    ftmParam.firstEdgeDelayPercent = 0U;

    FTM_GetDefaultConfig(&ftmInfo);
    /* Initialize FTM module */
    FTM_Init(APP_FTM_BASEADDR, &ftmInfo);

    FTM_SetupPwm(APP_FTM_BASEADDR, &ftmParam, 1U, kFTM_CenterAlignedPwm, 400U, CLOCK_GetFreq(kCLOCK_TimerClk));

    /* Enable channel interrupt flag.*/
    //FTM_EnableInterrupts(APP_FTM_BASEADDR, kFTM_Chnl3InterruptEnable);

    /* Enable at the NVIC */
	//EnableIRQ(FTM2_IRQn);

	FTM_StartTimer(APP_FTM_BASEADDR, kFTM_SystemClock);
#else
	pwm_t pwm_config;
	gpio_pin_config_t pwm = {kGPIO_DigitalOutput, 0,};
	GPIO_PinInit(APP_PWM_PORT, APP_PWM_PIN, &pwm);
	time_pwm = 50; 				/* millissegundos period */
	pwm_config.duty_cycle = 5; 	/* duty cycle 25 % */
	pwm_config.status = false;	/* status pwm  */
	app_pwm_set(pwm_config);
	app_timer_pit_init(time_pwm,0);
	app_timer_pit_callback(0,app_pwm_irq);
	app_timer_pit_start(0);

#if APP_PWM_TASK_DEBUG
	PRINTF("\n\r[APPPWM]PWM init SD");
#endif
#endif /* APP_TYPE_PWM */


}

/**
 *
 */
pwm_t app_pwm_get(void)
{
	return param_pwm;
}

/**
 *
 */
void app_pwm_set(pwm_t st)
{
	param_pwm =  st;
}

/**
 *
 */
void app_pwm_start(void)
{
	param_pwm.status = true;
}

/**
 *
 */
void app_pwm_stop(void)
{
	param_pwm.status = false;
}

/**
 *
 */
void app_pwm_duty_cycle(uint8_t dc)
{
	param_pwm.duty_cycle = dc;
}

/**
 *
 */
#if APP_TYPE_PWM

void FTM2_IRQHandler(void)
{
	if ((FTM_GetStatusFlags(APP_FTM_BASEADDR) & kFTM_Chnl3Flag) == kFTM_Chnl3Flag)
	{
		/* Clear interrupt flag.*/
		FTM_ClearStatusFlags(APP_FTM_BASEADDR, kFTM_Chnl3Flag);
	}
}
#else

void app_pwm_irq(void)
{
	static bool status_io = false;
	uint8_t duty_cycle = 0;
	if(param_pwm.status == true)
	{
		count_pwm++;
//		if(param_pwm.duty_cycle > 50)
//		{
//			duty_cycle = 50;
//		}
//		else
//		{
			duty_cycle = param_pwm.duty_cycle + 1;
//		}
		//if(count_pwm < ((float)time_pwm*((float)param_pwm.duty_cycle/100.00F)))
		if(count_pwm < ((float)time_pwm*((float)duty_cycle/100.00F)))
		//if(count_pwm < ((float)time_pwm*((float)duty_cycle/500.00F)))
		{
			if(status_io == false)
			{
				status_io = true;
				GPIO_PinWrite(APP_PWM_PORT, APP_PWM_PIN,1);
			}
		}
		else
		{
			if(status_io == true)
			{
				status_io = false;
				GPIO_PinWrite(APP_PWM_PORT, APP_PWM_PIN,0);
			}
		}
		if(count_pwm > APP_PWM_CYCLE)
		{
			count_pwm = 0;
		}
	}
	else
	{
		count_pwm = 0;
		if(status_io == true)
		{
			status_io = false;
			GPIO_PinWrite(APP_PWM_PORT, APP_PWM_PIN,0);
		}
	}
}

#endif /* APP_TYPE_PWM */
