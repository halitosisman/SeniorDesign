/*
 * net_task.c
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 *
 *
 *      Much of the initialization code here was copied from the Simplelink CC32xx SDK tcpecho example
 */


#include <tasks/lcd_task/lcd_task.h>


static volatile uint32_t * const wrx = (uint32_t *) (GPIOA0_BASE + LCD_WRX_MSK);
static volatile uint32_t * const dcx = (uint32_t *) (GPIOA0_BASE + LCD_DCX_MSK);
static volatile uint32_t * const data_03 = (uint32_t *) (GPIOA0_BASE + LCD_D74_MSK);
static volatile uint32_t * const data_47 = (uint32_t *) (GPIOA1_BASE + LCD_D30_MSK);

QueueHandle_t sl_event_box;


inline void set_dcx(uint8_t val) {
    *dcx = val;
}


/*
    Note: I took a look at the generated assembly for this, and it looks like the nominal execution time for the loop
    here is 262.5ns.
*/
void FG_write_bulk(uint8_t * buf, uint16_t cnt) {

    for(int i = 0; i < cnt; i++) {
        *data_03 = buf[i] << 4;
        *data_47 = buf[i] >> 4;
        *wrx = 0xFF;

        __asm("    nop");
        __asm("    nop");

        *wrx = 0x0;
    }
}


void FG_write_color(uint8_t * color, uint8_t color_len, int num) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < color_len; j++) {
            *data_03 = color[j] << 4;
            *data_47 = color[j] >> 4;
            *wrx = 0xFF;

            __asm("    nop");
            __asm("    nop");

            *wrx = 0x0;
        }
    }
}


void write8(uint8_t val) {
    *data_03 = val << 4;
    *data_47 = val >> 4;

    *wrx = 0xFF;

    __asm("    nop");
    __asm("    nop");

    *wrx = 0x0;
}


int32_t fatfs_getFatTime(void) {
    return 0;
}


void lcd_task(void * par) {
    GPIO_init();

#if 0
#define
    *wrx = 0x0;
    *data_03 = 0x0;
    *data_47 = 0x0;

    ili_init();
    lcd_clear();
#endif

    FIL* fp;

    SDFatFS_init();
    SDFatFS_open(0, 0);

    FRESULT r = f_open(fp, "test.c", (BYTE) 'w');
    if (!r) {
        while (1) {
        }

    }
    char in[] = "Hello, World!";
    char out[20];
    UINT bw;
    UINT btw = sizeof(in);
    r = f_write(fp, in, btw, &bw);
    if (!r) {
        while (1) {
        }

    }

    r = f_close(fp);
    if (!r) {
        while (1) {
        }

    }

    r = f_open(fp, "test.c", (BYTE) 'r');
    if (!r) {
        while (1) {
        }

    }
    r = f_write(fp, in, btw, &bw);
    if (!r) {
        while (1) {
        }

    }

    while (1) {
    }

}
