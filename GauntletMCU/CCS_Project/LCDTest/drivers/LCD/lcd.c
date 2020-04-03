/*
 * lcd.c
 *
 *  Created on: Feb 26, 2020
 *      Author: pbloemer
 */


#include "lcd.h"


LCD_CFG pg_set =
{
     .cmd = ILI9341_PAGEADDRSET,
     small = {0, 0, 0, 0},
     .tag = FOUR_BYTE_CMD
};

LCD_CFG cl_set =
{
     .cmd = ILI9341_COLADDRSET,
     .data.small = {0, 0, 0, 0},
     .tag = FOUR_BYTE_CMD
};

LCD_CFG mem_write =
{
     .cmd = ILI9341_MEMORYWRITE,
     .data.small = {0, 0, 0, 0},
     .tag = NO_CMD
};


void lcd_init() {
    LCD_CFG reset;
    LCD_CFG disp_off;
    LCD_CFG pow1;
    LCD_CFG pow2;
    LCD_CFG vcom1;
    LCD_CFG vcom2;
    LCD_CFG memctrl;
    LCD_CFG pixformat;
    LCD_CFG frctrl;
    LCD_CFG slp_out;
    LCD_CFG disp_on;

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
    memctrl.data.small[0] = ILI9341_MADCTL_MY | ILI9341_MADCTL_MX |
            ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
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

    //mem_write.data.bulk = blah;
    // mem_write.tag = sizeof(blah);

    ILI_cfg(reset);
    vTaskDelay(pdMS_TO_TICKS(5));
    ILI_cfg(disp_off);
    ILI_cfg(pow1);
    ILI_cfg(pow2);
    ILI_cfg(vcom1);
    ILI_cfg(vcom2);
    ILI_cfg(memctrl);
    ILI_cfg(pixformat);
    ILI_cfg(frctrl);
    ILI_cfg(slp_out);
    ILI_cfg(disp_on);
    vTaskDelay(pdMS_TO_TICKS(500));

    //ILI_write(mem_write);
}


// possibly deprecated
void lcd_clear() {
    uint8_t color[] = {0x0, 0x0};

    pg_set.data[0] = 0x0;
    pg_set.data[1] = 0x0;
    pg_set.data[2] = 0x0;
    pg_set.data[3] = 0xEF;

    cl_set.data[0] = 0x0;
    cl_set.data[1] = 0x0;
    cl_set.data[2] = 0x1;
    cl_set.data[3] = 0x3F;

    ILI_cfg(pg_set);
    ILI_cfg(cl_set);

    ILI_cfg(mem_write);
    ILI_write_color((uint8_t *) color, sizeof(color), LCD_COLUMNS * LCD_ROWS);
}
