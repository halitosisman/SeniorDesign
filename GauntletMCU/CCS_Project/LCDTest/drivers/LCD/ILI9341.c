/*
 * lcd.c
 *
 *  Created on: Feb 21, 2020
 *      Author: pbloemer
 *
 *
 *      Initialization code largely copied from
 *       https://github.com/adafruit/TFTLCD-Library/blob/master/Adafruit_TFTLCD.cpp for source on initialization
 *
 */


#include "ILI9341.h"


void ILI_cfg(LCD_CFG setting)
{
    set_dcx(DCX_CMD);
    write8(setting.cmd);
    set_dcx(DCX_DATA);
    switch (setting.tag) {
    case TWO_BYTE_CMD:
        write8(setting.data[0]);
        write8(setting.data[1]);
        break;
    case ONE_BYTE_CMD:
        write8(setting.data[0]);
        break;
    case FOUR_BYTE_CMD:
        ILI_write_bulk((uint8_t *) setting.data, setting.tag);
        break;
    case NO_CMD:
        break;
    default:
        // maybe add a bounds check here
    }
}

