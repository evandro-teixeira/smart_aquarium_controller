/*
 * AppSmartAquaConfig.h
 *
 *  Created on: 20/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPSMARTAQUACONFIG_H_
#define APPSMARTAQUA_APPSMARTAQUACONFIG_H_

#include "AppShell.h"
#include "AppIO.h"
#include "AppTimer.h"
#include "AppPwm.h"
#include "AppRtc.h"
#include "AppTemperature.h"
#include "AppLCD16x2.h"
#include "AppDisplay.h"
#include "MKE06Z4.h"
#include "AppControll.h"
#include "AppIhm.h"

/* Config Task */
#define APP_PRIORITIES_HIGH 		(configMAX_PRIORITIES - 1)
#define APP_PRIORITIES_MED 			(configMAX_PRIORITIES - 2)
#define APP_PRIORITIES_LOW 			(configMAX_PRIORITIES - 3)
#define APP_SHELL_TASK_STATCK 		configMINIMAL_STACK_SIZE * 3
#define APP_TEMP_TASK_STATCK 		configMINIMAL_STACK_SIZE * 2
#define APP_IHM_TASK_STATCK 		configMINIMAL_STACK_SIZE * 3
#define APP_DISPLAY_TASK_STATCK 	configMINIMAL_STACK_SIZE * 3
#define APP_CONTROLL_TASK_STATCK	configMINIMAL_STACK_SIZE * 2
#define APP_IO_TASK_STATCK			configMINIMAL_STACK_SIZE * 2

#define APP_TEMP_TASK_DEBUG			0
#define APP_SHELL_TASK_DEBUG		0
#define APP_IO_TASK_DEBUG			0
#define APP_PWM_TASK_DEBUG			0
#define APP_MAIN_TASK_DEBUG			1
#define APP_IHM_TASK_DEBUG			0
#define APP_DISPLAY_TASK_DEBUG		0
#define APP_CONTROLL_TASK_DEBUG     1

/* Number of Relay */
#define APP_NUMBER_OF_RELAY			3

/* UART instance and clock */
#define APP_SHELL_UART 				UART2
#define APP_SHELL_UART_CLK_FREQ 	CLOCK_GetFreq(kCLOCK_BusClk)
#define APP_SHELL_UART_IRQn 		UART2_IRQn
#define APP_SHELL_BAUDRATE 			115200
#define APP_SHELL_IRQHandler		UART2_IRQHandler

/* LED RGB */
#define APP_LED_RED_PORT 			kGPIO_PORTG
#define APP_LED_RED_PIN				5U
#define APP_LED_GREEN_PORT			kGPIO_PORTG
#define APP_LED_GREEN_PIN			6U
#define APP_LED_BLUE_PORT 			kGPIO_PORTG
#define APP_LED_BLUE_PIN			7U

/* Reles */
#define APP_RELE1_PORT				kGPIO_PORTC
#define APP_RELE1_PIN				0U
#define APP_RELE2_PORT				kGPIO_PORTF
#define APP_RELE2_PIN				7U
#define APP_RELE3_PORT				kGPIO_PORTG
#define APP_RELE3_PIN				1U

/* Inputs Chs */
//#define APP_CH_UP_PORT			kGPIO_PORTC
//#define APP_CH_UP_PIN				7U
//#define APP_CH_UP_KBI0_PIN 		23U
#define APP_CH_UP_PORT				kGPIO_PORTB
#define APP_CH_UP_PIN				0U
#define APP_CH_UP_KBI0_PIN 			8U

//#define APP_CH_DOWN_PORT			kGPIO_PORTC
//#define APP_CH_DOWN_PIN			6U
//#define APP_CH_DOWN_KBI0_PIN 		22U
#define APP_CH_DOWN_PORT			kGPIO_PORTD
#define APP_CH_DOWN_PIN				5U
#define APP_CH_DOWN_KBI0_PIN 		29U

#define APP_CH_LEFT_PORT			kGPIO_PORTD
#define APP_CH_LEFT_PIN				7U
#define APP_CH_LEFT_KBI0_PIN 		31U

#define APP_CH_RIGHT_PORT			kGPIO_PORTD
#define APP_CH_RIGHT_PIN			6U
#define APP_CH_RIGHT_KBI0_PIN 		30U

#define APP_CH_ENTER_PORT			kGPIO_PORTH
#define APP_CH_ENTER_PIN			3U
#define APP_CH_ENTER_KBI1_PIN 		27U

#define APP_CH_BACK_PORT			kGPIO_PORTH
#define APP_CH_BACK_PIN				4U
#define APP_CH_BACK_KBI1_PIN 		28U

/* Config KBI */
#define APP_KBI0 					KBI0
#define APP_KBI1 					KBI1

/* The edge selection for each kbi pin */
#define APP_EDGE_FALLING 			0 // Falling
#define APP_EDGE_RINSING 			1 // Rinsing


/* */
#define APP_IO_MAX_LENGTH			8
#define APP_ADC_MAX_LENGTH			8
#define APP_SHELL_MAX_LENGTH		8
#define APP_DISPLAY_MAX_LENGTH		4

/*  */
#define APP_SHELL_MAX_SIZE_BUFFER	32

/* Config PWM */
#define APP_PWM_TYPE				0
#define APP_PWM_PORT				kGPIO_PORTD
#define APP_PWM_PIN					1U

//#define APP_TPM_CHANNEL			kTPM_Chnl_3 //3	// PTD1 PWM1
//#define APP_SD_ON_LEVEL 			kTPM_LowTrue
//#define APP_TPM_BASEADDR 			TPM0

#define APP_FTM2_BASEADDR 			FTM2
#define APP_FTM_CHANNEL 			kFTM_Chnl_3

/* Interrupt number and interrupt handler for the FTM instance used */
#define APP_FTM_INTERRUPT_NUMBER 	FTM2_IRQn
//#define FTM_LED_HANDLER 			FTM2_IRQHandler

/* Interrupt to enable and flag to read; depends on the FTM channel used */
#define APP_FTM_CHANNEL_INTERRUPT_ENABLE 	kFTM_Chnl3InterruptEnable
#define FTM_CHANNEL_FLAG 					kFTM_Chnl3Flag

/* */
//#ifndef ptrTask
//typedef void(*ptrTask)(void) ;
//#endif

#define APP_TEMPERATURE_BASE 		ADC
#define APP_TEMPERATURE_CHANNEL 	9U
#define APP_TEMPERATURE_SIZE_BUFFER	8
#define APP_TEMPERATURE_DIGITAL		0
#define APP_TEMPERATURE_ANALOG		1

#define APP_VER_FIRMWARE			(char*)("Smart Aqua V2.0")


/* define App Control */
#define APP_TIME_STATE_INIT			2000  /* 02 secunds */
#define APP_TIME_STATE_MAIN_STATUS	5000  /* 05 secunds */
#define APP_TIMEOUT					30000 /* 30 secunds */

/* The Flextimer instance/channel used for board */
#define APP_FTM2_BASEADDR 			FTM2

/* The Flextimer instance/channel used for board */
#define APP_FTM1_BASEADDR 			FTM0


#define APP_SIZE_BUFFER_LCD			(uint8_t)(LCD_NUMBER_CH + 1)

/**
 *
 */
#define APP_LCD_PIN		\
{						\
	{ /* PTF1 - RS */	\
		kGPIO_PORTF,	\
		1U				\
	},					\
	{ /* PTF0 - RW */	\
		kGPIO_PORTF,	\
		0U				\
	},					\
	{ /* PTD2 - EN */	\
		kGPIO_PORTD,	\
		2U				\
	},					\
	{ /* PTG1 - D4 */	\
		kGPIO_PORTG,	\
		1U				\
	},					\
	{ /* PTG3 - D5 */	\
		kGPIO_PORTG,	\
		3U				\
	},					\
	{ /* PTG2 - D6 */	\
		kGPIO_PORTG,	\
		2U				\
	},					\
	{ /* PTG0 - D7 */	\
		kGPIO_PORTG,	\
		0U				\
	},					\
}

#define APP_LCD_NUMBER_PIN	7

#define APP_IO_KEY_PIN		\
{							\
	{						\
		APP_CH_DOWN_PORT,	\
		APP_CH_DOWN_PIN		\
	},						\
	{						\
		APP_CH_UP_PORT,		\
		APP_CH_UP_PIN		\
	},						\
	{						\
		APP_CH_LEFT_PORT,	\
		APP_CH_LEFT_PIN		\
	},						\
	{						\
		APP_CH_RIGHT_PORT,	\
		APP_CH_RIGHT_PIN	\
	},						\
	{						\
		APP_CH_ENTER_PORT,	\
		APP_CH_ENTER_PIN	\
	},						\
	{						\
		APP_CH_BACK_PORT,	\
		APP_CH_BACK_PIN		\
	}						\
}

/* KBI */
#define APP_KBI_ENABLE		0

#define APP_HIGH			1
#define APP_LOW				0

/* SD */
#define APP_TIME_SD_AUTO	1

#define APP_PWM_CYCLE		250// PWM cycle

#define APP_CONFIGURED_TIME_OFF	0x01 // Configured
#define APP_CONFIGURED_TIME_ON 	0x02
#define APP_CONFIGURED_TIME_SD 	0x04

#define APP_LED_SD			RGB_Red
#define APP_LED_RELAY_1		RGB_Yellow
#define APP_LED_RELAY_2		RGB_Green
#define APP_LED_RELAY_3		RGB_Cyan
#define APP_NUMBER_LED		4
/*
Blue
Magenta
Write
*/
/*
#define APP_LED_INDEX	\
{						\
	RGB_Red,			\
	APP_LED_RELAY_1,	\
	APP_LED_RELAY_2,	\
	APP_LED_RELAY_1		\
}
*/
#endif /* APPSMARTAQUA_APPSMARTAQUACONFIG_H_ */
