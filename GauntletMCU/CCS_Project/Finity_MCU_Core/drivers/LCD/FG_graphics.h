/*
 * FG_graphics.h
 *
 *  Created on: Feb 27, 2020
 *      Author: halitosisman
 */

/*!****************************************************************************
 *  @file       FG_graphics.h
 *  @brief      Asynchronous I2C transaction driver
 *
 *  Ports ILI9341 and grlib functions to the
 *  LCD, https://www.adafruit.com/product/2090, and the TI CC3220 respectively.
 *  This driver bit bangs GPIOS to form an 8 bit parallel interface with a
 *  tested speed of approximately 1.5 MHz. There is much room for optimization.
 *
 *  ***************************************************************************/

#ifndef DRIVERS_LCD_FG_GRAPHICS_H_
#define DRIVERS_LCD_FG_GRAPHICS_H_

#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_gpio.h>

#include <ti/grlib/grlib.h>
#include <ti/grlib/checkbox.h>

#include "drivers/LCD/lcd.h"
#include "drivers/LCD/ILI9341.h"

// GPIO address masks for the pins used by this driver
#define LCD_D74_MSK 0x3c0
#define LCD_D30_MSK 0x3c
#define LCD_WRX_MSK 0x4
#define LCD_DCX_MSK 0x20

// GPIO data pins
#define LCD_DBUS_HIGH (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define LCD_DBUS_LOW (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)

/*
 * grlib uses 8-8-8 bit RGB colors and the ILI9341 uses 5-6-5 RGB colors. These masks drop the low bits of the grlib
 * colors to make them ILI9341 friendly.
 */
#define FGG_RED_MSK 0xF80000
#define FGG_GREEN_MSK 0xFC00
#define FGG_BLUE_MSK 0xF8

/*
 * This is the only Graphics_Context instance visible to grlib as it is currently written. The grlib functions could be
 * easily modified to actually use their Graphics_Context paramters.
 */
extern Graphics_Context g_sContext;

#ifdef __cplusplus
extern "C" {
#endif

extern const Graphics_Font g_sFontCm16; // The font used in the Finity Gauntlet.

/*!
 *  @brief  Initializes the LCD and grlib port.
 */
extern void FG_graphics_init();

/*!
 *  @brief  Prints some primitives on the screen. Used for testing.
 */
extern void FG_graphics_test();

#ifdef __cplusplus
}
#endif


#endif /* DRIVERS_LCD_FG_GRAPHICS_H_ */
