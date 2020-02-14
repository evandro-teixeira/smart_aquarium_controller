/*
 * AppTimer.h
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPTIMER_H_
#define APPSMARTAQUA_APPTIMER_H_

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "AppSmartAquaConfig.h"
//#include "fsl_kbi.h"
#include "fsl_ftm.h"
#include "fsl_pit.h"

#ifndef ptrTask
typedef void(*ptrTask)(void) ;
#endif

bool app_timer_pit_init(uint32_t time /* millisecond period */, uint8_t ch);
void app_timer_pit_start(uint8_t ch);
void app_timer_pit_stop(uint8_t ch);
void app_timer_pit_callback(uint8_t ch,ptrTask task);

bool app_timer_ftm_init(FTM_Type *timer, uint32_t time);
bool app_timer_ftm_start(FTM_Type *timer);
//void app_timer_ftm_callback(ptrTask task);
bool app_timer_ftm_callback(FTM_Type *timer,ptrTask task);


#endif /* APPSMARTAQUA_APPTIMER_H_ */
