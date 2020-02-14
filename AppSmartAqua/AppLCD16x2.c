/*
 * AppLCD16x2.c
 *
 *  Created on: 27/09/2019
 *      Author: evandro
 */

#include "AppLCD16x2.h"

/**
 *
 */
typedef struct
{
	gpio_port_num_t port;
	uint32_t pin;
}lcd_pins_t;

/**
 *
 */
typedef union
{
	uint8_t byte;
	struct
	{
		unsigned bit0 : 1;
		unsigned bit1 : 1;
		unsigned bit2 : 1;
		unsigned bit3 : 1;
		unsigned bit4 : 1;
		unsigned bit5 : 1;
		unsigned bit6 : 1;
		unsigned bit7 : 1;
	};
}lcd_data_t;

/**
 *
 */
typedef enum
{
	lcd_rs = 0,
	lcd_rw,
	lcd_en,
	lcd_d4,
	lcd_d5,
	lcd_d6,
	lcd_d7,
}lcd_index_t;

/**
 *
 */
const lcd_pins_t lcd_pins[APP_LCD_NUMBER_PIN] = APP_LCD_PIN;
static char lcd_modo1 = 0;
static char lcd_modo2 = 0;

/**
 *
 */
static void app_lcd_delay(uint32_t t);
static void app_lcd_nibble_send(uint8_t dt);
static void app_lcd_byte_send(uint8_t en,uint8_t data);
static void app_lcd_xy(uint8_t x, uint8_t y);
static void app_lcd_write_char(char c);

/**
 *
 */
void app_lcd_init(void)
{
	gpio_pin_config_t lcd = {kGPIO_DigitalOutput, 0,};
	uint8_t i = 0;

	for(i=0;i<APP_LCD_NUMBER_PIN;i++)
	{
		GPIO_PinInit(lcd_pins[i].port, lcd_pins[i].pin, &lcd);
	}

	app_lcd_delay(5000);
	app_lcd_nibble_send(0x03);
	app_lcd_delay(2500);
	app_lcd_nibble_send(0x03);
	app_lcd_delay(2500);
	app_lcd_nibble_send(0x03);
	app_lcd_delay(2500);
	app_lcd_nibble_send(0x02);
	app_lcd_delay(1500);

	app_lcd_byte_send(0,0x02);
	app_lcd_delay(1500);

	lcd_modo1 = (char)(0x20 | 0x00 | 0x08);

	app_lcd_byte_send(0,0x20 | 0x00 | 0x08);
	app_lcd_delay(1500);

	lcd_modo2 = (char)(0x08 | 0x04 | 0x00 | 0x00);

	app_lcd_byte_send(0, lcd_modo2);
	app_lcd_delay(1500);

	app_lcd_byte_send(0,0x01);
	app_lcd_delay(1500);

	app_lcd_byte_send(0,0x06);
	app_lcd_delay(1500);
}

/**
 *
 */
static void app_lcd_delay(uint32_t t)
{
	t /= 1;
	while(t--){}
}

/**
 *
 */
static void app_lcd_nibble_send(uint8_t dt)
{
	lcd_data_t data;
	data.byte = dt;

	// Data D4
	GPIO_PinWrite(lcd_pins[lcd_d4].port, lcd_pins[lcd_d4].pin,data.bit0);
	// Data D5
	GPIO_PinWrite(lcd_pins[lcd_d5].port, lcd_pins[lcd_d5].pin,data.bit1);
	// Data D6
	GPIO_PinWrite(lcd_pins[lcd_d6].port, lcd_pins[lcd_d6].pin,data.bit2);
	// Data D7
	GPIO_PinWrite(lcd_pins[lcd_d7].port, lcd_pins[lcd_d7].pin,data.bit3);

	// Data EN
	GPIO_PinWrite(lcd_pins[lcd_en].port, lcd_pins[lcd_en].pin, 1 );
	app_lcd_delay(50);
	GPIO_PinWrite(lcd_pins[lcd_en].port, lcd_pins[lcd_en].pin, 0 );
}

/**
 *
 */
static void app_lcd_byte_send(uint8_t en,uint8_t data)
{
	// Data RW
	GPIO_PinWrite(lcd_pins[lcd_rw].port, lcd_pins[lcd_rw].pin, 0 );

	// configura a linha rs dependendo do modo selecionado:
	// 0 -> registrador de comando
	// 1 -> registrador de dados
//	if(en)
//	{
		// Data RS
		GPIO_PinWrite(lcd_pins[lcd_rs].port, lcd_pins[lcd_rs].pin, (en==0)?0:1 );
//	}
//	else
//	{
		// Data RS
//		GPIO_PinWrite(lcd_pins[lcd_rs].port, lcd_pins[lcd_rs].pin, 0 );
//	}

	app_lcd_delay(50);

	// Data EN
	GPIO_PinWrite(lcd_pins[lcd_en].port, lcd_pins[lcd_en].pin, 0 );

	// Envia LSB
	app_lcd_nibble_send(data >> 4);
	// Envia MSB
	app_lcd_nibble_send(data & 0x0F);

	app_lcd_delay(50);
}

/**
 *
 */
static void app_lcd_xy(uint8_t x, uint8_t y)
{
	uint8_t end = 0;

	if(y != 1)
	{
		end = 0xC0;
	}

	end += (x-1);
	end |= 0x80;

	app_lcd_byte_send(0,end);
	app_lcd_delay(5);
}

/**
 *
 */
static void app_lcd_write_char(char c)
{
	switch(c)
	{
		case '\f':
			app_lcd_byte_send(0x00,0x01);
			app_lcd_delay(750);
		break;
		case '\n':
		case '\r':
			app_lcd_byte_send(0x01,0x02);
		break;
		default:
			app_lcd_byte_send(0x01,c);
			app_lcd_delay(5);
	}
}

/**
 *
 */
void app_lcd_write_string(uint8_t line,char *c)
{
	if(line == 0)
	{
		app_lcd_xy(0,1);
	}
	else
	{
		app_lcd_xy(0,2);
	}

	while(*c)
	{
		app_lcd_write_char(*c);
		c++;
	}
}
