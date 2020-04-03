/*
 * lcd.h
 *
 *  Created on: Feb 21, 2020
 *      Author: pbloemer
 */

#ifndef DRIVERS_LCD_ILI9341_H_
#define DRIVERS_LCD_ILI9341_H_


#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"


#ifdef _cplusplus
#define
extern "C" {
#endif


/*
 *  These defines ripped from https://github.com/adafruit/TFTLCD-Library/blob/master/registers.h
 */

#define ILI9341_BITS_PER_RED 5
#define ILI9341_BITS_PER_GREEN 6
#define ILI9341_BITS_PER_BLUE 5


#define ILI9341_SOFTRESET 0x01
#define ILI9341_SLEEPIN 0x10
#define ILI9341_SLEEPOUT 0x11
#define ILI9341_NORMALDISP 0x13
#define ILI9341_INVERTOFF 0x20
#define ILI9341_INVERTON 0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPLAYOFF 0x28
#define ILI9341_DISPLAYON 0x29
#define ILI9341_COLADDRSET 0x2A
#define ILI9341_PAGEADDRSET 0x2B
#define ILI9341_MEMORYWRITE 0x2C
#define ILI9341_PIXELFORMAT 0x3A
#define ILI9341_FRAMECONTROL 0xB1
#define ILI9341_DISPLAYFUNC 0xB6
#define ILI9341_ENTRYMODE 0xB7
#define ILI9341_POWERCONTROL1 0xC0
#define ILI9341_POWERCONTROL2 0xC1
#define ILI9341_VCOMCONTROL1 0xC5
#define ILI9341_VCOMCONTROL2 0xC7
#define ILI9341_MEMCONTROL 0x36
#define ILI9341_MADCTL 0x36

#define ILI9341_MADCTL_MY 0x80
#define ILI9341_MADCTL_MX 0x40
#define ILI9341_MADCTL_MV 0x20
#define ILI9341_MADCTL_ML 0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH 0x04

#define DCX_CMD 0x0
#define DCX_DATA 0xFF

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


/*!
    \brief          write8

    Callback for writing a single byte to the ILI9341

    \param          uint8_t    -   The byte to be written

    \return         void

    \note           The user needs to define a custom implementation of this for a particular MCU. The FG project has a
                    implementation of this under lcd_task.c.

*/
void write8(uint8_t val);
void ILI_write_bulk(uint8_t * buf, uint16_t cnt);
void ILI_write_color(uint8_t * color, uint8_t color_len, int num);


/*!
    \brief          set_dcx

    Callback for setting the dcx line high or low.

    \param          uint8_t    -   0xFF for command and 0x0 for data.

    \return         void

    \note           The user needs to define a custom implementation of this for a particular MCU. The FG project has a
                    implementation of this under lcd_task.c.

*/
inline void set_dcx(uint8_t val);


/*!
    \brief          ILI_cfg

    More efficient routine for writing command - parameter sequences

    \param          LCD_CMD_t   -   The bytes to be written in order of low index to high

    \return         void

    \note

*/
void ILI_cfg(LCD_CFG setting);


#ifdef _cplusplus
#define
extern }
#endif



#endif /* DRIVERS_LCD_ILI9341_H_ */
