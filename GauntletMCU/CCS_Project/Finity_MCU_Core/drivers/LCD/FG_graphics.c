/*
 * FG_graphics.c
 *
 *  Created on: Feb 27, 2020
 *      Author: halitosisman
 *
 *
 */


#include "drivers/LCD/FG_graphics.h"


// GPIO data registers
static volatile uint32_t * const wrx = (uint32_t *) (GPIOA0_BASE + LCD_WRX_MSK);
static volatile uint32_t * const dcx = (uint32_t *) (GPIOA0_BASE + LCD_DCX_MSK);
static volatile uint32_t * const data_30 = (uint32_t *) (GPIOA1_BASE + LCD_D30_MSK);
static volatile uint32_t * const data_74 = (uint32_t *) (GPIOA0_BASE + LCD_D74_MSK);

// grlib callbacks
static void pfnPixelDraw(const Graphics_Display * pDisplay, int16_t lX, int16_t lY, uint16_t ulValue);
static void pfnPixelDrawMultiple(const Graphics_Display * pDisplay, int16_t lX, int16_t lY, int16_t lX0, int16_t lCount,
                          int16_t lBPP, const uint8_t * pucData, const uint32_t * pucPalette);
static void pfnLineDrawH(const Graphics_Display * pDisplay, int16_t lX1, int16_t lX2, int16_t lY, uint16_t ulValue);
static void pfnLineDrawV(const Graphics_Display * pDisplay, int16_t lX, int16_t lY1, int16_t lY2, uint16_t ulValue);
static void pfnRectFill(const Graphics_Display * pDisplay, const Graphics_Rectangle * pRect, uint16_t ulValue);
static uint32_t pfnColorTranslate(const Graphics_Display * pDisplay, uint32_t ulValue);
static void pfnFlush(const Graphics_Display * pDisplay);
static void pfnClearDisplay(const Graphics_Display * pDisplay, uint16_t ulValue);


// A bunch of the grlib stuff is initialized here
static Graphics_Display_Functions FGG_Functions =
{
     .pfnPixelDraw = pfnPixelDraw,
     .pfnPixelDrawMultiple = pfnPixelDrawMultiple,
     .pfnLineDrawH = pfnLineDrawH,
     .pfnLineDrawV = pfnLineDrawV,
     .pfnRectFill = pfnRectFill,
     .pfnColorTranslate = pfnColorTranslate,
     .pfnFlush = pfnFlush,
     .pfnClearDisplay = pfnClearDisplay
};

static Graphics_Display wrist_display =
{
     .displayData = NULL,
     .heigth = LCD_ROWS,
     .pFxns = &FGG_Functions,
     .size = sizeof(Graphics_Display),
     .width = LCD_COLUMNS
};

Graphics_Context g_sContext =
{
     .background = 0,
     .clipRegion.xMin = 0,
     .clipRegion.yMin = 0,
     .clipRegion.xMax = LCD_COLUMNS - 1,
     .clipRegion.yMax = LCD_ROWS - 1,
     .display = &wrist_display,
     .font = &g_sFontCm16,
     .foreground = 0,
     .size = sizeof(Graphics_Context),
     .background = 0
};

inline void set_dcx(uint8_t val) {
    *dcx = val;
}


/*
    Note: I took a look at the generated assembly for this, and it looks like the nominal execution time for the loop
    here is 262.5ns.
*/
void ILI_write_bulk(uint8_t * buf, uint16_t cnt) {

    for(int i = 0; i < cnt; i++) {
        *data_30 = buf[i]; // write
        *data_74 = buf[i];
        *wrx = 0xFF; // clock high

        __asm("    nop"); // Wait for clk/data lines to stop panicking
        __asm("    nop");

        *wrx = 0x0; // clock low
    }
}


void ILI_write_color(uint8_t * color, uint8_t color_len, int num) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < color_len; j++) {
            *data_30 = color[j];
            *data_74 = color[j];
            *wrx = 0xFF;

            __asm("    nop");
            __asm("    nop");

            *wrx = 0x0;
        }
    }
}


void write8(uint8_t val) {
    *data_30 = val;
    *data_74 = val;

    *wrx = 0xFF;

    __asm("    nop");
    __asm("    nop");

    *wrx = 0x0;
}


static void pfnPixelDraw(const Graphics_Display * pDisplay, int16_t lX, int16_t lY, uint16_t ulValue) {
    pg_set.data[0] = lY >> 8;
    pg_set.data[1] = lY & 0xFF;
    pg_set.data[2] = (lY + 1) >> 8;
    pg_set.data[3] = (lY + 1) & 0xFF;

    cl_set.data[0] = lX >> 8;
    cl_set.data[1] = lX & 0xFF;
    cl_set.data[2] = (lX + 1) >> 8;
    cl_set.data[3] = (lX + 1) & 0xFF;

    ILI_cfg(pg_set);
    ILI_cfg(cl_set);

    ILI_cfg(mem_write);
    write8((uint8_t) (ulValue >> 8));
    write8((uint8_t) (ulValue & 0xFF));
}


static void pfnPixelDrawMultiple(const Graphics_Display * pDisplay, int16_t lX, int16_t lY, int16_t lX0, int16_t lCount,
                          int16_t lBPP, const uint8_t * pucData, const uint32_t * pucPalette) {
    int16_t x0 = lX0 / lBPP;
    uint16_t display_color = 0;
    uint32_t graphics_color = 0;
    uint8_t palette_msk = 0;

    pg_set.data[0] = lY >> 8;
    pg_set.data[1] = lY & 0xFF;
    pg_set.data[2] = lY>> 8;
    pg_set.data[3] = lY & 0xFF;;

    cl_set.data[0] = lX >> 8;
    cl_set.data[1] = lX & 0xFF;
    cl_set.data[2] = (lX + lCount) >> 8;  // Note that the column raster is one larger than necessary here
    cl_set.data[3] = (lX + lCount) & 0xFF;

    ILI_cfg(pg_set);
    ILI_cfg(cl_set);

    ILI_cfg(mem_write);

    palette_msk = 0xF >> (8 - lBPP);

    for(int16_t i = x0; i < lCount; i++) {
        graphics_color = pucPalette[pucData[i] >> (i % (8 / lBPP)) & palette_msk];
        display_color = pDisplay->pFxns->pfnColorTranslate(pDisplay, graphics_color);
        write8(display_color >> 8);
        write8(display_color & 0xFF);
    }
}


static void pfnLineDrawH(const Graphics_Display * pDisplay, int16_t lX1, int16_t lX2, int16_t lY, uint16_t ulValue) {
    uint8_t buf[] = {ulValue >> 8, ulValue & 0xFF};

    pg_set.data[0] = lY >> 8;
    pg_set.data[1] = lY & 0xFF;
    pg_set.data[2] = lY>> 8;
    pg_set.data[3] = lY & 0xFF;;

    cl_set.data[0] = lX1 >> 8;
    cl_set.data[1] = lX1 & 0xFF;
    cl_set.data[2] = lX2 >> 8;
    cl_set.data[3] = lX2 & 0xFF;

    ILI_cfg(pg_set);
    ILI_cfg(cl_set);

    ILI_cfg(mem_write);

    ILI_write_color(buf, sizeof(buf), lX2 - lX1 + 1);
}


static void pfnLineDrawV(const Graphics_Display * pDisplay, int16_t lX, int16_t lY1, int16_t lY2, uint16_t ulValue) {
    uint8_t buf[] = {ulValue >> 8, ulValue & 0xFF};

    pg_set.data[0] = lY1 >> 8;
    pg_set.data[1] = lY1 & 0xFF;
    pg_set.data[2] = lY2 >> 8;
    pg_set.data[3] = lY2 & 0xFF;;

    cl_set.data[0] = lX >> 8;
    cl_set.data[1] = lX & 0xFF;
    cl_set.data[2] = lX >> 8;
    cl_set.data[3] = lX & 0xFF;

    ILI_cfg(pg_set);
    ILI_cfg(cl_set);

    ILI_cfg(mem_write);

    ILI_write_color(buf, sizeof(buf), lY2 - lY1 + 1);
}


static void pfnRectFill(const Graphics_Display * pDisplay, const Graphics_Rectangle * pRect, uint16_t ulValue) {
    uint8_t buf[] = {ulValue >> 8, ulValue & 8};
    int size = 0;

    pg_set.data[0] = pRect->yMin >> 8;
    pg_set.data[1] = pRect->yMin & 0xFF;
    pg_set.data[2] = pRect->yMax >> 8;
    pg_set.data[3] = pRect->yMax & 0xFF;;

    cl_set.data[0] = pRect->xMin >> 8;
    cl_set.data[1] = pRect->xMin & 0xFF;
    cl_set.data[2] = pRect->xMax >> 8;
    cl_set.data[3] = pRect->xMax & 0xFF;

    ILI_cfg(pg_set);
    ILI_cfg(cl_set);

    ILI_cfg(mem_write);

    size = (pRect->yMax - pRect->yMin + 1) * (pRect->xMax - pRect->xMin + 1);

    ILI_write_color(buf, sizeof(buf), size);
}


// Translates ulValue to 5-6-5 format by discarding less significant bits
static uint32_t pfnColorTranslate(const Graphics_Display * pDisplay, uint32_t ulValue) {
    uint16_t retVal = 0;
    retVal = ((ulValue & FGG_RED_MSK) >> 8) | ((ulValue & FGG_GREEN_MSK) >> 5) | ((ulValue & FGG_BLUE_MSK) >> 3);
    return retVal;
}


static void pfnFlush(const Graphics_Display * pDisplay) {

}


static void pfnClearDisplay(const Graphics_Display * pDisplay, uint16_t ulValue) {
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
    ILI_write_color((uint8_t *) color, sizeof(color), pDisplay->width * pDisplay->heigth);
}

void FG_graphics_init() {
    *dcx = DCX_CMD;
    *wrx = 0x0;
    *data_30 = 0x0;
    *data_74 = 0x0;
    lcd_init();
    Graphics_clearDisplay(&g_sContext);
}

void FG_graphics_test() {
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_drawPixel(&g_sContext, 10, 5);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawLineH(&g_sContext, 10, 300, 10);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME);
    Graphics_drawLineV(&g_sContext, 315, 10, 220);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_PURPLE);
    Graphics_fillCircle(&g_sContext, 100, 100, 15);

    int8_t hi[] = "Hello, World!";
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_AQUAMARINE);
    Graphics_drawString(&g_sContext, hi, sizeof(hi) - 1, 10, 200, 1);

    int8_t checkText[] = "Checkbox!";

    Graphics_CheckBox testBox =
    {
     .backgroundColor = GRAPHICS_COLOR_DEEP_SKY_BLUE,
     .font = &g_sFontCm12,
     .gap = 5,
     .numbOfChar = sizeof(checkText) - 1,
     .selected = 0,
     .selectedColor = GRAPHICS_COLOR_DARK_VIOLET,
     .text = checkText,
     .textColor = GRAPHICS_COLOR_INDIAN_RED,
     .xPosition = 200,
     .yPosition = 200
    };
    Graphics_drawCheckBox(&g_sContext, &testBox);
    Graphics_clearDisplay(&g_sContext);
}
