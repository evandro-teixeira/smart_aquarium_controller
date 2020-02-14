/*
 * AppPwm.h
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPPWM_H_
#define APPSMARTAQUA_APPPWM_H_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "AppSmartAquaConfig.h"
#include "fsl_kbi.h"
#include "fsl_ftm.h"

typedef struct
{
	uint8_t duty_cycle;
	bool status;
}pwm_t;

void app_pwm_init(void);
pwm_t app_pwm_get(void);
void app_pwm_set(pwm_t st);
void app_pwm_start(void);
void app_pwm_stop(void);
void app_pwm_duty_cycle(uint8_t dc);

#endif /* APPSMARTAQUA_APPPWM_H_ */
