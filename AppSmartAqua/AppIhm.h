/*
 * AppIhm.h
 *
 *  Created on: 03/10/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPIHM_H_
#define APPSMARTAQUA_APPIHM_H_


#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "AppSmartAquaConfig.h"

void app_ihm_init_task(void);

#endif /* APPSMARTAQUA_APPIHM_H_ */
