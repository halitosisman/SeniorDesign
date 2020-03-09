/*
 * FG_graphics.c
 *
 *  Created on: Feb 27, 2020
 *      Author: halitosisman
 */


#include "drivers/LCD/FG_graphics.h"


static Graphics_Rectangle wrist_display_limits =
{
     .xMax = LCD_COLUMNS - 1,
     .xMin = 0,
     .yMax = LCD_ROWS - 1,
     .yMin = 0
};

static Graphics_Display wrist_display =
{
     .displayData = NULL,
     .heigth = LCD_ROWS,
     .pFxns = NULL, // TODO Define these.
     .size = sizeof(Graphics_Display),
     .width = LCD_COLUMNS
};

#if 0
#define
Graphics_Context wrist_context =
{
     .background = 0,
     .clipRegion = wr_display_limits,
     .font = g_sFontCm12,
     .foreground = 0,
     .size = sizeof(Graphics_Context),
     .display = wrist_display
};
#endif

void pfnPixelDraw(const Graphics_Display * pDisplay, int16_t lX, int16_t lY, uint16_t ulValue) {
    mem_write.tag = NO_CMD;

    pg_set.data.small[0] = 0x0;
    pg_set.data.small[1] = 0x0;
    pg_set.data.small[2] = lX >> 8;
    pg_set.data.small[3] = lX & 0xF;

    cl_set.data.small[0] = 0x0;
    cl_set.data.small[1] = 0x0;
    cl_set.data.small[2] = 0x0;
    cl_set.data.small[3] = 0xEF;
}


void FG_graphics_init() {
    lcd_init();
    lcd_clear();


}
