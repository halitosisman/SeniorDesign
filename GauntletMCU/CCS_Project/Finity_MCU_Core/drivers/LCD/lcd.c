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
     .data = {0, 0, 0, 0},
     .tag = FOUR_BYTE_CMD
};

LCD_CFG cl_set =
{
     .cmd = ILI9341_COLADDRSET,
     .data = {0, 0, 0, 0},
     .tag = FOUR_BYTE_CMD
};

LCD_CFG mem_write =
{
     .cmd = ILI9341_MEMORYWRITE,
     .data = {0, 0, 0, 0},
     .tag = NO_CMD
};


static LCD_CFG reset =
{
     .cmd = ILI9341_SOFTRESET,
     .data[0] = 0x0,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG disp_off =
{
     .cmd = ILI9341_DISPLAYOFF,
     .data[0] = 0x0,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG pow1 =
{
     .cmd = ILI9341_POWERCONTROL1,
     .data[0] = 0x23,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG pow2 =
{
     .cmd = ILI9341_POWERCONTROL2,
     .data[0] = 0x10,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG vcom1 =
{
     .cmd = ILI9341_VCOMCONTROL1,
     .data = {0x2B, 0x2B, 0x0, 0x0},
     .tag = TWO_BYTE_CMD
};

static LCD_CFG vcom2 =
{
     .cmd = ILI9341_VCOMCONTROL2,
     .data[0] = 0xC0,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG memctrl =
{
     .cmd = ILI9341_MEMCONTROL,
     .data[0] = ILI9341_MADCTL_BGR | ILI9341_MADCTL_MV,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG pixformat =
{
     .cmd = ILI9341_PIXELFORMAT,
     .data[0] = 0x55,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG frctrl =
{
     .cmd = ILI9341_FRAMECONTROL,
     .data = {0x00, 0x1B, 0x0, 0x0},
     .tag = TWO_BYTE_CMD
};

static LCD_CFG slp_out =
{
     .cmd = ILI9341_SLEEPOUT,
     .data[0] = 0x0,
     .tag = ONE_BYTE_CMD
};

static LCD_CFG disp_on =
{
     .cmd = ILI9341_DISPLAYON,
     .data[0] = 0x0,
     .tag = ONE_BYTE_CMD
};


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
        break;
        // maybe add a bounds check here
    }
}


void lcd_init() {

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
