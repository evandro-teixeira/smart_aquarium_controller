/*
 * AppTimer.c
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */
#include "AppTimer.h"

/**
 *
 */
static ptrTask task_timer_pit[kPIT_Chnl_2];
static ptrTask task_timer_ftm0;
static ptrTask task_timer_ftm1;
static ptrTask task_timer_ftm2;

/**
 *
 */
bool app_timer_pit_init(uint32_t time, uint8_t ch)
{
	if(ch > kPIT_Chnl_1) return false;

    /* Structure of initialize PIT */
    pit_config_t pitConfig;
    /*
     * pitConfig.enableRunInDebug = false;
     */
    PIT_GetDefaultConfig(&pitConfig);

    /* Init pit module */
    PIT_Init(PIT, &pitConfig);

    /* Set timer period for channel  */
    PIT_SetTimerPeriod(PIT, (pit_chnl_t)ch,USEC_TO_COUNT/*MSEC_TO_COUNT*/(time, CLOCK_GetFreq(kCLOCK_BusClk)));

    /* Enable timer interrupts for channel  */
    PIT_EnableInterrupts(PIT, (pit_chnl_t)ch, kPIT_TimerInterruptEnable);

    /* Enable at the NVIC */
    if(ch < kPIT_Chnl_2)
    {
    	EnableIRQ(PIT_CH0_IRQn);
    }
    else
    {
    	EnableIRQ(PIT_CH1_IRQn);
    }

	return true;
}

/**
 *
 */
void app_timer_pit_start(uint8_t ch)
{
	if(ch > kPIT_Chnl_3) return;
	PIT_StartTimer(PIT, (pit_chnl_t)ch);
}

/**
 *
 */
void app_timer_pit_stop(uint8_t ch)
{
	if(ch > kPIT_Chnl_3) return;
	PIT_StopTimer(PIT, (pit_chnl_t)ch);
}

/**
 *
 */
void app_timer_pit_callback(uint8_t ch,ptrTask task)
{
	if( (ch > kPIT_Chnl_1) && (task == NULL) ) return;
	task_timer_pit[ch] = task;
}

/**
 *
 */
void PIT_CH0_IRQHandler(void)
{
	uint8_t i = 0;
	for(i=0;i<2;i++)
	{
		if(PIT_GetStatusFlags(PIT,i))
		{
			PIT_ClearStatusFlags(PIT, i, kPIT_TimerFlag);
			task_timer_pit[i]();
		}
	}
}

/**
 *
 */
void PIT_CH1_IRQHandler(void)
{
	uint8_t i = 0;
	for(i=0;i<2;i++)
	{
		if(PIT_GetStatusFlags(PIT,i))
		{
			PIT_ClearStatusFlags(PIT, i, kPIT_TimerFlag);
			task_timer_pit[(i+2)]();
		}
	}
}

/**
 *
 */
bool app_timer_ftm_init(FTM_Type *timer,uint32_t time)
{
	ftm_config_t ftmInfo;

	if((timer != FTM0) && (timer != FTM1) && (timer != FTM2))
	{
		return false;
	}
	else
	{
		FTM_GetDefaultConfig(&ftmInfo);

		/* Divide FTM clock by 4 */
		ftmInfo.prescale = kFTM_Prescale_Divide_4;

//		/* Initialize FTM module */
//		FTM_Init(/*APP_FTM2_BASEADDR*/timer, &ftmInfo);
//
//		/* Set timer period. */
//		FTM_SetTimerPeriod(/*APP_FTM2_BASEADDR*/timer, USEC_TO_COUNT(time, (CLOCK_GetFreq(kCLOCK_TimerClk)/4)));
//
//		FTM_EnableInterrupts(/*APP_FTM2_BASEADDR*/timer, kFTM_TimeOverflowInterruptEnable);

		//ftmInfo.faultMode = kFTM_Fault_EvenChnls;

		if(timer == FTM0)
		{
			/* Initialize FTM module */
			FTM_Init(FTM0, &ftmInfo);

			/* Set timer period. */
			FTM_SetTimerPeriod(FTM0, USEC_TO_COUNT(time, (CLOCK_GetFreq(kCLOCK_TimerClk)/4)));

			FTM_EnableInterrupts(FTM0, kFTM_TimeOverflowInterruptEnable);

			EnableIRQ(FTM0_IRQn);
		}
		else if(timer == FTM1)
		{
			/* Initialize FTM module */
			FTM_Init(FTM1, &ftmInfo);

			/* Set timer period. */
			FTM_SetTimerPeriod(FTM1, USEC_TO_COUNT(time, (CLOCK_GetFreq(kCLOCK_TimerClk)/4)));

			FTM_EnableInterrupts(FTM1, kFTM_TimeOverflowInterruptEnable);

			EnableIRQ(FTM1_IRQn);
		}
		else
		{
			/* Initialize FTM module */
			FTM_Init(FTM2, &ftmInfo);

			/* Set timer period. */
			FTM_SetTimerPeriod(FTM2, USEC_TO_COUNT(time, (CLOCK_GetFreq(kCLOCK_TimerClk)/4)));

			FTM_EnableInterrupts(FTM2, kFTM_TimeOverflowInterruptEnable);

			EnableIRQ(FTM2_IRQn);
		}
	}
    return true;
}

/**
 *
 */
bool app_timer_ftm_start(FTM_Type *timer)
{
	if((timer != FTM0) && (timer != FTM1) && (timer != FTM2))
	{
		return false;
	}
	else
	{
		if(timer == FTM0)
		{
			FTM_StartTimer(FTM0, kFTM_SystemClock);
		}
		else if(timer == FTM1)
		{
			FTM_StartTimer(FTM1, kFTM_SystemClock);
		}
		else
		{
			FTM_StartTimer(FTM2, kFTM_SystemClock);
		}
	}
	return true;
}

/**
 *
 */
bool app_timer_ftm_callback(FTM_Type *timer,ptrTask task)
{
	if((timer != FTM0) && (timer != FTM1) && (timer != FTM2))
	{
		return false;
	}
	else
	{
		if(task != NULL)
		{
			if(timer == FTM0)
			{
				task_timer_ftm0 = task;
			}
			else if(timer == FTM1)
			{
				task_timer_ftm1 = task;
			}
			else
			{
				task_timer_ftm2 = task;
			}
		}
	}
	return true;
}

/**
 *
 */
void FTM0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    FTM_ClearStatusFlags(FTM0, kFTM_TimeOverflowFlag);
    task_timer_ftm0();
}

/**
 *
 */
void FTM1_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    FTM_ClearStatusFlags(FTM1, kFTM_TimeOverflowFlag);
    task_timer_ftm1();
}

/**
 *
 */
void FTM2_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    FTM_ClearStatusFlags(APP_FTM2_BASEADDR, kFTM_TimeOverflowFlag);
    task_timer_ftm2();
#if APP_KBI_ENABLE
    /* Gambiara */
    app_io_ticks_irq();
#endif /* APP_KBI_ENABLE */
}
