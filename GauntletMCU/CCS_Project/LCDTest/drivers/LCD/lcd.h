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


typedef uint8_t tag_t;


/*!
 *  @brief      Template transaction with LCD
 *
 *  Note that small style transactions are MSB first and bulk transactions are LSB
 *
 */
typedef struct _LCD_CFG {
    /*  Determines the category of transaction.  */
    tag_t tag;

    /*  command code */
    uint8_t cmd;

    /* command parameters */
    uint8_t data[4];
} LCD_CFG;


extern LCD_CFG pg_set;
extern LCD_CFG cl_set;
extern LCD_CFG mem_write;

/*!
    \brief          ILI_cfg

    More efficient routine for writing command - parameter sequences

    \param          LCD_CMD_t   -   The bytes to be written in order of low index to high

    \return         void

    \note

*/
void ILI_cfg(LCD_CFG setting);


//void set_draw_window(int16_t lX, int16_t lY, );
void lcd_init();
void lcd_clear();


#ifdef _cplusplus
#define
extern }
#endif

#endif /* DRIVERS_LCD_LCD_H_ */
