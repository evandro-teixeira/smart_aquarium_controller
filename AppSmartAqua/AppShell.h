/*
 * AppShell.h
 *
 *  Created on: 20/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPSHELL_H_
#define APPSMARTAQUA_APPSHELL_H_

#include <string.h>
#include <stdio.h>

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_uart.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_shell.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "AppSmartAquaConfig.h"

void app_shell_init_task(void);
void app_shell_queue_create(void);

#endif /* APPSMARTAQUA_APPSHELL_H_ */
