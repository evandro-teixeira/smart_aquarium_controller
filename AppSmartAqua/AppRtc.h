/*
 * AppRtc.h
 *
 *  Created on: 25/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPRTC_H_
#define APPSMARTAQUA_APPRTC_H_

#include "AppSmartAquaConfig.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_rtc.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"

void app_rtc_init(void);
rtc_datetime_t app_rtc_datetime_get(void);
void app_rtc_update(rtc_datetime_t date);

#endif /* APPSMARTAQUA_APPRTC_H_ */
