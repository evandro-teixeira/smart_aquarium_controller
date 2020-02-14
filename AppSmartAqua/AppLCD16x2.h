/*
 * AppLCD16x2.h
 *
 *  Created on: 27/09/2019
 *      Author: evandro
 */

#ifndef APPSMARTAQUA_APPLCD16X2_H_
#define APPSMARTAQUA_APPLCD16X2_H_

#include "AppSmartAquaConfig.h"

#define LCD_NUMBER_CH		16
#define LCD_NUMBER_LINE		2

void app_lcd_init(void);
void app_lcd_write_string(uint8_t line,char *c);

#endif /* APPSMARTAQUA_APPLCD16X2_H_ */
