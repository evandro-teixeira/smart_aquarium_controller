/*
 * AppIO.c
 *
 *  Created on: 21/09/2019
 *      Author: evandro
 */

#include "AppIO.h"

/**
 *
 */
#if APP_KBI_ENABLE
static QueueHandle_t io_irq_queue = NULL;
static uint32_t ticks = 0;
#else
const io_pins_t io_key_pins[key_max] = APP_IO_KEY_PIN;
#endif /* APP_KBI_ENABLE */

static QueueHandle_t io_queue = NULL;
static io_param_t param_io = {0};
//static rgb_state_t led_status_app[APP_NUMBER_LED] =  APP_LED_INDEX;
/**
 *
 */

#if APP_KBI_ENABLE
//static void app_io_ticks_irq(void);
static uint32_t app_io_ticks_get(void);
#endif
//static void app_io_pwm_init(void);
static void app_io_task(void *pvParameters);
static void app_io_rgb_controll(rgb_state_t state);

void app_io_init_task(void)
{
	if(xTaskCreate(app_io_task,"Io_task",APP_IO_TASK_STATCK, NULL, APP_PRIORITIES_HIGH, NULL) != pdPASS)
	{
#if APP_IO_TASK_DEBUG
		PRINTF("\n\r[APP_IO]Manager_task Create\n\r");
#endif /* APP_MANAGER_TASK_DEBUG */

	}
}

/**
 *
 */
static void app_io_task(void *pvParameters)
{

#if APP_KBI_ENABLE
	//static bool state_io[key_max] = {false};
	//io_data_state_t io = {0};
	io_irq_data_t data_in = {0};
	io_data_t data_out = {0};
	io_ctrl_t io[key_max] = {0};
	bool flag_queue = false;
#else

	io_ctrl_t debounce_key[key_max] = {{0},{0}};
	io_data_t data_out = {0};
	bool flag_queue = false;
	uint8_t index_pin = 0;
	//uint8_t index_rgb = 0;
	//uint32_t pin = 0;
	uint16_t counter_rgb = 0;
	uint8_t state_rgb = 0;
#endif /* APP_KBI_ENABLE */

	app_io_init();

	while(1)
	{

#if APP_KBI_ENABLE
		if( xQueueReceive(io_irq_queue,&data_in,(TickType_t) 0xFFFFFFFF) == pdTRUE )
		{

#if APP_IO_TASK_DEBUG
	//PRINTF("\n\r");
	PRINTF("\n\r[APP_IO]Receive Queue I/O ");
	PRINTF("\n\r[APP_IO]Value: %d ", data_in.io_data.data);
	PRINTF("\n\r[APP_IO]Time: %u ", data_in.time);
#endif	/* APP_IO_TASK_DEBUG */

			if(data_in.io_data.key.down == 0)
			{
				io[key_down].time = data_in.time;
				io[key_down].state = 1;
			}
			else
			{
				if(io[key_down].state == 1)
				{
					io[key_down].state = 0;
					if((data_in.time) > (io[key_down].time + 100))
					{
						data_out.key.down = 1;
						flag_queue = true;
					}
				}
			}

			/* send queue */
			if(flag_queue == true)
			{
				if( xQueueSend( io_queue, (void *) &data_out,(TickType_t) 0 ) != pdPASS )
				{
#if APP_IO_TASK_DEBUG
	PRINTF("\n\r[APP_IO]Send Fail Queue I/O ");
#endif	/* APP_IO_TASK_DEBUG */
				}
				else
				{
#if APP_IO_TASK_DEBUG
	PRINTF("\n\r[APP_IO]SendQueue I/O ");
#endif	/* APP_IO_TASK_DEBUG */
				}
			}
		}
#else

		/* Clear */
		data_out.data = 0;

		/* Read key */
		for(index_pin=0;index_pin < key_max;index_pin++)
		{
			//pin = GPIO_PinRead(io_key_pins[index_pin].port,io_key_pins[index_pin].pin);
			if(GPIO_PinRead(io_key_pins[index_pin].port,io_key_pins[index_pin].pin) == APP_LOW)
			//pin = GPIO_PinRead(APP_CH_ENTER_PORT,APP_CH_ENTER_PIN);
			//if(pin == APP_LOW)
			{
				debounce_key[index_pin].time++;
				debounce_key[index_pin].state = true;
			}
			else
			{
				if(debounce_key[index_pin].state == true)
				{
					if(debounce_key[index_pin].time >= 2)
					{
						data_out.data |= 1 << index_pin;
						flag_queue = true;
					}
					debounce_key[index_pin].state = false;
					debounce_key[index_pin].time = 0;
				}
			}
		}

		/* send queue */
		if(flag_queue == true)
		{
			flag_queue = false;

#if APP_IO_TASK_DEBUG
PRINTF("\n\r[APP_IO]Value: %b ",data_out.data);
#endif	/* APP_IO_TASK_DEBUG */

			if( xQueueSend( io_queue, (void *) &data_out,(TickType_t) 0 ) != pdPASS )
			{
#if APP_IO_TASK_DEBUG
PRINTF("\n\r[APP_IO]Send Fail Queue I/O ");
#endif	/* APP_IO_TASK_DEBUG */
			}
			else
			{
#if APP_IO_TASK_DEBUG
PRINTF("\n\r[APP_IO]SendQueue I/O ");
#endif	/* APP_IO_TASK_DEBUG */
			}
		}

		/**
		 * Controle led RGB
		 */
		if(counter_rgb < 50)
		{
			counter_rgb++;
		}
		else
		{
			counter_rgb = 0;
			switch(state_rgb)
			{
				case 0:
					if((app_manager_state_sd_get() == sd_turning_on) || (app_manager_state_sd_get() == sd_on))
					{
						app_io_rgb_controll(RGB_Red);
					}
					else
					{
						app_io_rgb_controll(RGB_Off);
					}
					state_rgb = 1;
				break;
				case 1:
					//if(app_manager_status_relay_get(0 /* Relay 1 */) == io_on)
					if(app_io_relay1_get()== io_on)
					{
						app_io_rgb_controll(RGB_Yellow);
					}
					else
					{
						app_io_rgb_controll(RGB_Off);
					}
					state_rgb = 2;
				break;
				case 2:
					//if(app_manager_status_relay_get(1 /* Relay 2 */) == io_on)
					if(app_io_relay2_get()== io_on)
					{
						app_io_rgb_controll(RGB_Green);
					}
					else
					{
						app_io_rgb_controll(RGB_Off);
					}
					state_rgb = 3;
				break;
				case 3:
					//if(app_manager_status_relay_get(2 /* Relay 3 */) == io_on)
					if(app_io_relay3_get()== io_on)
					{
						app_io_rgb_controll(RGB_Cyan);
					}
					else
					{
						app_io_rgb_controll(RGB_Off);
					}
					state_rgb = 0;
				break;
				default:
					state_rgb = 0;
				break;
			}
		}

        vTaskDelay((TickType_t)25 /* 10 Millessecunds */);
#endif /* APP_KBI_ENABLE */

	}
}

/**
 *
 */
void app_io_queue_create(void)
{
	io_queue = xQueueCreate(APP_IO_MAX_LENGTH,sizeof(io_data_t) );
	if(io_queue)
	{
#if APP_IO_TASK_DEBUG
	PRINTF("\n\r[APP_IO]Create Queue I/O ");
#endif	/* APP_IO_TASK_DEBUG */
	}

#if APP_KBI_ENABLE
	io_irq_queue = xQueueCreate(APP_IO_MAX_LENGTH,sizeof(io_irq_data_t));
	if(io_irq_queue)
	{
#if APP_IO_TASK_DEBUG
	PRINTF("\n\r[APP_IO]Create Queue I/O IRq");
#endif	/* APP_IO_TASK_DEBUG */
	}
#endif /* APP_KBI_ENABLE */
}

/**
 *
 */
bool app_io_queue_receive(io_data_t *data, uint32_t time)
{
	bool ret = false;
	if( xQueueReceive(io_queue,data,(TickType_t) time) == pdTRUE )
	{
		ret = true;
#if APP_IO_TASK_DEBUG
	PRINTF("\n\r[APP_IO]Receive Queue I/O ");
#endif	/* APP_IO_TASK_DEBUG */
	}

	return ret;
}
/**
 *
 */
void app_io_init(void)
{
#if APP_IO_TASK_DEBUG
	PRINTF("\n\r[APP_IO]Init I/Os ");
#endif	/* APP_IO_TASK_DEBUG */

    gpio_pin_config_t led = 	{kGPIO_DigitalOutput, 1,};
    gpio_pin_config_t rele = 	{kGPIO_DigitalOutput, 0,};
    gpio_pin_config_t key = 	{kGPIO_DigitalInput,  0,};
    /* LED RGB Init */
    GPIO_PinInit(APP_LED_RED_PORT, APP_LED_RED_PIN,     &led);
    GPIO_PinInit(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN, &led);
    GPIO_PinInit(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,   &led);
    /* Rele Init */
    GPIO_PinInit(APP_RELE1_PORT, APP_RELE1_PIN, &rele);
    GPIO_PinInit(APP_RELE2_PORT, APP_RELE2_PIN, &rele);
    GPIO_PinInit(APP_RELE3_PORT, APP_RELE3_PIN, &rele);
    /* SW's Init */
#if APP_KBI_ENABLE /* KBI Disable */
    PORT_SetFilterSelect(PORT, kPORT_FilterKBI0,kPORT_BUSCLK_OR_NOFILTER/* kPORT_FILTERDIV1 */);
    PORT_SetFilterSelect(PORT, kPORT_FilterKBI1,kPORT_BUSCLK_OR_NOFILTER/* kPORT_FILTERDIV1 */);
#endif /* KBI Disable */
    GPIO_PinInit(APP_CH_DOWN_PORT,  APP_CH_DOWN_PIN,  &key);
    GPIO_PinInit(APP_CH_UP_PORT,    APP_CH_UP_PIN,    &key);
    GPIO_PinInit(APP_CH_LEFT_PORT,  APP_CH_LEFT_PIN,  &key);
    GPIO_PinInit(APP_CH_RIGHT_PORT, APP_CH_RIGHT_PIN, &key);
    GPIO_PinInit(APP_CH_ENTER_PORT, APP_CH_ENTER_PIN, &key);
    GPIO_PinInit(APP_CH_BACK_PORT,  APP_CH_BACK_PIN,  &key);

#if APP_KBI_ENABLE /* KBI Disable */
    kbi_config_t kbiConfig;
    /* Config KBI0 */
    kbiConfig.mode = kKBI_EdgesLevelDetect; //kKBI_EdgesLevelDetect;// kKBI_EdgesDetect;
    kbiConfig.pinsEnabled = (1 << APP_CH_UP_KBI0_PIN) 	|
    						(1 << APP_CH_DOWN_KBI0_PIN)	|
							(1 << APP_CH_LEFT_KBI0_PIN)	|
							(1 << APP_CH_RIGHT_KBI0_PIN);
    //kbiConfig.pinsEdge =  APP_EDGE_RINSING;
    //kbiConfig.pinsEdge = 0xFFFFFFFF;
    kbiConfig.pinsEdge = 	(1 << APP_CH_UP_KBI0_PIN) 	|
    						(1 << APP_CH_DOWN_KBI0_PIN)	|
							(1 << APP_CH_LEFT_KBI0_PIN)	|
							(1 << APP_CH_RIGHT_KBI0_PIN);
    KBI_Init(APP_KBI0, &kbiConfig);

    /* Config KBI1 */
    kbiConfig.mode = kKBI_EdgesLevelDetect; //kKBI_EdgesLevelDetect;// kKBI_EdgesDetect;
    kbiConfig.pinsEnabled = (1 << APP_CH_ENTER_KBI1_PIN) |
	  						(1 << APP_CH_BACK_KBI1_PIN);
    //kbiConfig.pinsEdge =  APP_EDGE_RINSING;
    //kbiConfig.pinsEdge = 0xFFFFFFFF;
    kbiConfig.pinsEdge |= 	(1 << APP_CH_ENTER_KBI1_PIN) |
							(1 << APP_CH_BACK_KBI1_PIN);
    KBI_Init(APP_KBI1, &kbiConfig);

    //app_timer_ftm_init(APP_FTM1_BASEADDR, 1000 /* 01 millissecunds */);
    //app_timer_ftm_callback(APP_FTM1_BASEADDR, app_io_ticks_irq);
    //app_timer_ftm_start(APP_FTM1_BASEADDR);
    //app_io_pwm_init();
#endif /* APP_KBI_ENABLE */

}

/**
 *
 */
#if APP_KBI_ENABLE
void app_io_ticks_irq(void)
{
	ticks++;
}
#endif /* APP_KBI_ENABLE */
/**
 *
 */
#if APP_KBI_ENABLE
static uint32_t app_io_ticks_get(void)
{
	return ticks;
}
#endif /* APP_KBI_ENABLE */

/**
 *
 */
//static void app_io_pwm_init(void)
//{
//	// PTD1_PWM3
//
//	PORT_SetPinSelect(kPORT_FTM2CH3, kPORT_FTM2_CH3_PTD1);     /* pin  is configured as  */
//
//    ftm_config_t ftmInfo;
//    ftm_chnl_pwm_signal_param_t ftmParam;
//    ftm_pwm_level_select_t pwmLevel = kFTM_LowTrue;
//
//   app_sd_duty_cycle = 2; // 50 %
//
//    /* Configure ftm params with frequency 24kHZ */
//    ftmParam.chnlNumber = APP_FTM_CHANNEL;
//    ftmParam.level = pwmLevel;
//    ftmParam.dutyCyclePercent = app_sd_duty_cycle;
//    ftmParam.firstEdgeDelayPercent = 0U;
//
//    FTM_GetDefaultConfig(&ftmInfo);
//    /* Initialize FTM module */
//    FTM_Init(APP_FTM_BASEADDR, &ftmInfo);
//
//    FTM_SetupPwm(APP_FTM_BASEADDR, &ftmParam, 1U, kFTM_CenterAlignedPwm, 400U, CLOCK_GetFreq(kCLOCK_TimerClk));
//
//    /* Enable channel interrupt flag.*/
//    //FTM_EnableInterrupts(APP_FTM_BASEADDR, kFTM_Chnl3InterruptEnable);
//
//    /* Enable at the NVIC */
//	//EnableIRQ(FTM2_IRQn);
//
//	FTM_StartTimer(APP_FTM_BASEADDR, kFTM_SystemClock);
//}

/**
 *
 */
void KBI0_IRQHandler(void)
{
#if APP_KBI_ENABLE
	io_irq_data_t data = {0};
	data.io_data.data = 0xff;
#endif /* APP_KBI_ENABLE */

    if(KBI_IsInterruptRequestDetected(APP_KBI0))
    {
        /* Clear status. */
        KBI_ClearInterruptFlag(APP_KBI0);
#if APP_KBI_ENABLE
        /* Reads the current input */
        data.io_data.key.down	= GPIO_PinRead(APP_CH_DOWN_PORT,APP_CH_DOWN_PIN);
        data.io_data.key.up 	= GPIO_PinRead(APP_CH_UP_PORT,APP_CH_UP_PIN);
        data.io_data.key.left 	= GPIO_PinRead(APP_CH_LEFT_PORT,APP_CH_LEFT_PIN);
        data.io_data.key.right 	= GPIO_PinRead(APP_CH_RIGHT_PORT,APP_CH_RIGHT_PIN);
        data.time = app_io_ticks_get();

#if APP_IO_TASK_DEBUG
    PRINTF("\n\r");
	PRINTF("\n\r[APP_IO]%d %u",data.io_data.data, data.time);
#endif	/* APP_IO_TASK_DEBUG */

        /* Send msg with status I/Os */
        xQueueSendToFrontFromISR(io_irq_queue,&data,pdFALSE);
#endif /* APP_KBI_ENABLE */
    }
}

/**
 *
 */
void KBI1_IRQHandler(void)
{
#if APP_KBI_ENABLE
	io_irq_data_t data = {0};
	data.io_data.data = 0xff;
#endif /* APP_KBI_ENABLE */

    if(KBI_IsInterruptRequestDetected(APP_KBI1))
    {
        /* Clear status. */
        KBI_ClearInterruptFlag(APP_KBI1);

#if APP_KBI_ENABLE
        /* Reads the current input */
        data.io_data.key.enter = GPIO_PinRead(APP_CH_ENTER_PORT,APP_CH_ENTER_PIN);
        data.io_data.key.back = GPIO_PinRead(APP_CH_BACK_PORT,APP_CH_BACK_PIN);
        data.time = app_io_ticks_get();

#if APP_IO_TASK_DEBUG
    PRINTF("\n\r");
	PRINTF("\n\r[APP_IO]%d %u",data.io_data.data, data.time);
#endif	/* APP_IO_TASK_DEBUG */

        /* Send msg with status I/Os */
        xQueueSendToFrontFromISR(io_irq_queue,&data,pdFALSE);
#endif /* APP_KBI_ENABLE */

    }
}

/**
 *
 */
//void FTM2_IRQHandler(void)
//{
//	if ((FTM_GetStatusFlags(APP_FTM_BASEADDR) & kFTM_Chnl3Flag) == kFTM_Chnl3Flag)
//	{
//		/* Clear interrupt flag.*/
//		FTM_ClearStatusFlags(APP_FTM_BASEADDR, kFTM_Chnl3Flag);
//	}
//}

/**
 *
 */
io_param_t app_io_status_get(void)
{
	return param_io;
}

/**
 *
 */
void app_io_relay1_set(io_status_t st)
{
	param_io.relay1 = st;
	GPIO_PinWrite(APP_RELE1_PORT, APP_RELE1_PIN, st);
}

/**
 *
 */
void app_io_relay2_set(io_status_t st)
{
	param_io.relay2 = st;
	GPIO_PinWrite(APP_RELE2_PORT, APP_RELE2_PIN, st);
}

/**
 *
 */
void app_io_relay3_set(io_status_t st)
{
	param_io.relay3 = st;
	GPIO_PinWrite(APP_RELE3_PORT, APP_RELE3_PIN, st);
}

/**
 *
 */
io_status_t app_io_relay1_get(void)
{
	return param_io.relay1;
}

/**
 *
 */
io_status_t app_io_relay2_get(void)
{
	return param_io.relay2;
}

/**
 *
 */
io_status_t app_io_relay3_get(void)
{
	return param_io.relay2;
}

/**
 *
 */
static void app_io_rgb_controll(rgb_state_t state)
{
	switch(state)
	{
		case RGB_Red:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,0);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,1);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,1);
		break;
		case RGB_Yellow:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,0);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,0);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,1);
		break;
		case RGB_Green:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,1);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,0);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,1);
		break;
		case RGB_Cyan:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,1);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,0);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,0);
		break;
		case RGB_Blue:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,1);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,1);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,0);
		break;
		case RGB_Magenta:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,0);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,1);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,0);
		break;
		case RGB_Write:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,0);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,0);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,0);
		break;
		case RGB_Off:
		case RGB_WithotColor:
		default:
			GPIO_PinWrite(APP_LED_RED_PORT, APP_LED_RED_PIN,1);
			GPIO_PinWrite(APP_LED_GREEN_PORT, APP_LED_GREEN_PIN,1);
			GPIO_PinWrite(APP_LED_BLUE_PORT, APP_LED_BLUE_PIN,1);
		break;
	}
}
