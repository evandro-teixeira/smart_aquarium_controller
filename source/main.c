/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include <string.h>

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* SmartAqua include */
#include "../AppSmartAqua/AppSmartAquaConfig.h"

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
//static void hello_task(void *pvParameters)
//{
//	PRINTF("\n\r\n\rSmart Aqua Controller V2.0!!!\n\r");
//
//	app_shell_init_task();
//
//	for (;;)
//	{
//		/* Add your code here */
//		vTaskSuspend(NULL);
//	}
//}

/*!
 * @brief Application entry point.
 */
int main(void)
{
	/* Init board hardware. */
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	PRINTF("\n\r");
	PRINTF("\n\SMART AQUARIUM V2.0 ");
	/* Add your code here */
#if APP_MAIN_TASK_DEBUG
	//PRINTF("\n\r");
	PRINTF("\n\r[Main]App Init");
#endif /* APP_MAIN_TASK_DEBUG */

	//app_shell_init_task();
	app_io_init_task();
	app_display_init_task();
	app_temperature_init_task();
	app_controll_init_task();
	app_io_queue_create();
	app_shell_queue_create();
	app_display_queue_create();
	app_temperature_queue_create();
	app_ihm_init_task();
	vTaskStartScheduler();

	for(;;)
	{
		/* Infinite loop to avoid leaving the main function */
		/* something to use as a breakpoint stop while looping */
		__asm("NOP");

#if APP_MAIN_TASK_DEBUG
	PRINTF("\n\r[Main]FAIL RTOS");
#endif /* APP_MAIN_TASK_DEBUG */

	}
}

void HardFault_Handler(void)
{
#if APP_MAIN_TASK_DEBUG
	PRINTF("\n\r[Main]HardFault_Handler");
#endif /* APP_MAIN_TASK_DEBUG */

	while(1)
	{
		__asm("NOP");
	}
}


