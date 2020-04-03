/*
 * FG_graphics.h
 *
 *  Created on: Feb 27, 2020
 *      Author: halitosisman
 */

#ifndef DRIVERS_LCD_FG_GRAPHICS_H_
#define DRIVERS_LCD_FG_GRAPHICS_H_

#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_gpio.h>

#include <ti/grlib/grlib.h>

#include "drivers/LCD/lcd.h"
#include "drivers/LCD/ILI9341.h"


#define LCD_D74_MSK 0x3c0
#define LCD_D30_MSK 0x3c
#define LCD_WRX_MSK 0x4
#define LCD_DCX_MSK 0x20

#define LCD_DBUS_HIGH (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define LCD_DBUS_LOW (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)



#ifdef _cplusplus
#define
extern "C" {
#endif

extern const Graphics_Font g_sFontCm12;
extern Graphics_Context FGG_Context;

void FG_graphics_init();


#ifdef _cplusplus
#define
extern }
#endif


#endif /* DRIVERS_LCD_FG_GRAPHICS_H_ */
