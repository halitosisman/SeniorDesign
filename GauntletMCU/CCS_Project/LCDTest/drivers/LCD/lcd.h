/*
 * lcd.h
 *
 *  Created on: Feb 26, 2020
 *      Author: pbloemer
 */

#ifndef DRIVERS_LCD_LCD_H_
#define DRIVERS_LCD_LCD_H_

#include "drivers/LCD/ILI9341.h"


// Dependency: FG_graphics.h -> lcd.h -> chipset.h
// chipset.h callbacks need to be defined and then used to define lcd.h callbacks
#ifdef _cplusplus
#define
extern "C" {
#endif

#define LCD_COLUMNS 320
#define LCD_ROWS 240

#define LCD_MAX_COLUMN (LCD_COLUMNS - 1)
#define LCD_MAX_ROW (LCD_ROWS - 1)

#define NO_CMD 0
#define ONE_BYTE_CMD 1
#define TWO_BYTE_CMD 2
#define FOUR_BYTE_CMD 4

extern LCD_CFG pg_set;
extern LCD_CFG cl_set;
extern LCD_CFG mem_write;


//void set_draw_window(int16_t lX, int16_t lY, );
void lcd_init();
void lcd_clear();


#ifdef _cplusplus
#define
extern }
#endif

#endif /* DRIVERS_LCD_LCD_H_ */
