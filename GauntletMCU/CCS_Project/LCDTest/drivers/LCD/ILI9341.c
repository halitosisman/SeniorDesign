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


void ILI_write(LCD_CMD_t transaction)
{
    set_dcx(DCX_CMD);
    write8(transaction.cmd);
    set_dcx(DCX_DATA);
    switch (transaction.tag) {
    case TWO_BYTE_CMD:
        write8(transaction.data.small[0]);
        write8(transaction.data.small[1]);
        break;
    case ONE_BYTE_CMD:
        write8(transaction.data.small[0]);
        break;
    default: // TODO make the default behavior an error handler

        // If screen buffer sizes are 256 bytes and tag_t remains 8bits wide, than the loop condition check doubles as
        // a bounds check.
        for (tag_t i = 0; i < transaction.cmd; i++) {
            write8(transaction.data.bulk[i]);
        }
    }
}


void ili_init() {
    LCD_CMD_t reset;
    LCD_CMD_t disp_off;
    LCD_CMD_t pow1;
    LCD_CMD_t pow2;
    LCD_CMD_t vcom1;
    LCD_CMD_t vcom2;
    LCD_CMD_t memctrl;
    LCD_CMD_t pixformat;
    LCD_CMD_t frctrl;
    LCD_CMD_t slp_out;
    LCD_CMD_t disp_on;

    reset.cmd = ILI9341_SOFTRESET;
    reset.data.small[0] = 0x0;
    reset.tag = ONE_BYTE_CMD;

    disp_off.cmd = ILI9341_DISPLAYOFF;
    disp_off.data.small[0] = 0x0;
    disp_off.tag = ONE_BYTE_CMD;

    pow1.cmd = ILI9341_POWERCONTROL1;
    pow1.data.small[0] = 0x23;
    pow1.tag = ONE_BYTE_CMD;

    pow2.cmd = ILI9341_POWERCONTROL2;
    pow2.data.small[0] = 0x10;
    pow2.tag = ONE_BYTE_CMD;

    vcom1.cmd = ILI9341_VCOMCONTROL1;
    vcom1.data.small[0] = 0x2B;
    vcom1.data.small[1] = 0x2B;
    vcom1.tag = TWO_BYTE_CMD;

    vcom2.cmd = ILI9341_VCOMCONTROL2;
    vcom2.data.small[0] = 0xC0;
    vcom2.tag = ONE_BYTE_CMD;

    memctrl.cmd = ILI9341_MEMCONTROL;
    memctrl.data.small[0] = ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;
    memctrl.tag = ONE_BYTE_CMD;

    pixformat.cmd = ILI9341_PIXELFORMAT;
    pixformat.data.small[0] = 0x55;
    pixformat.tag = ONE_BYTE_CMD;

    frctrl.cmd = ILI9341_FRAMECONTROL;
    frctrl.data.small[0] = 0x00;
    frctrl.data.small[1] = 0x1B;
    frctrl.tag = TWO_BYTE_CMD;

    slp_out.cmd = ILI9341_SLEEPOUT;
    slp_out.data.small[0] = 0x0;
    slp_out.tag = ONE_BYTE_CMD;

    disp_on.cmd = ILI9341_DISPLAYON;
    disp_on.data.small[0] = 0x0;
    disp_on.tag = ONE_BYTE_CMD;

    ILI_write(reset);
    vTaskDelay(pdMS_TO_TICKS(5));
    ILI_write(disp_off);
    ILI_write(pow1);
    ILI_write(pow2);
    ILI_write(vcom1);
    ILI_write(vcom2);
    ILI_write(memctrl);
    ILI_write(pixformat);
    ILI_write(slp_out);
    vTaskDelay(pdMS_TO_TICKS(150));
    ILI_write(disp_on);
    vTaskDelay(pdMS_TO_TICKS(500));


#if 0
#define
    write8(ILI9341_SOFTRESET)
    //write8(0);
    vTaskDelay(pdMS_TO_TICKS(5));
    write8(ILI9341_DISPLAYOFF)
    //write8(0);

    write8(ILI9341_POWERCONTROL1);
    write8(0x23);
    write8(ILI9341_POWERCONTROL2);
    write8(0x10);
    write8(ILI9341_VCOMCONTROL1);
    write8(0x2B);
    write8(0x2B);
    write8(ILI9341_VCOMCONTROL2);
    write8(0xC0);
    write8(ILI9341_MEMCONTROL);
    write8(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
    write8(ILI9341_PIXELFORMAT);
    write8(0x55);
    write8(ILI9341_FRAMECONTROL);
    write8(0x00);
    write8(0x1B);

    //write8(ILI9341_ENTRYMODE, 0x07);

    write8(ILI9341_SLEEPOUT);
    //write8(0);
    vTaskDelay(pdMS_TO_TICKS(150));
    write8(ILI9341_DISPLAYON)
    //write8(0);
    vTaskDelay(pdMS_TO_TICKS(500));

    // TODO Addr window will need to be set
    //setAddrWindow(0, 0, TFTWIDTH - 1

#endif
}
