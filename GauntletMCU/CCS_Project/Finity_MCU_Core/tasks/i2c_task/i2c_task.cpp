/*
 * i2c_task.c
 *
 *  Created on: Apr 20, 2020
 *      Author: halitosisman
 */



#include "i2c_task.h"



void i2c_task(void * par) {
    Logger logger = Logger();
    State_Tracker state_tracker = State_Tracker();

    AD7993_Config FG_AD7993_Handle = // TODO Move to task file
    {
     .config = FINITY_GAUNTLET_AD7993_CONF,
     .cycle_timer = AD7993_CYCLE_TIMER_OFF,
     .data_high[0] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_high[1] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_high[2] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_high[3] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_low[0] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .data_low[1] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .data_low[2] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .data_low[3] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .hysteresis[0] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[1] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[2] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[3] = FINITY_GAUNTLET_AD7993_HYSTERESIS
    };

    Async_I2C_Handle * i2c_bus = AD7993_init(&FG_AD7993_Handle);
    I2C_Transaction * test;

    FG_GUI_init();
    logger.init();
    state_tracker.init();

    uint32_t p1;
    uint32_t p2;
    uint32_t r[4];

    char str0[20];
    char str1[20];
    char str2[20];
    char str3[20];

    int cx[4];

    while(1) {
        Graphics_clearDisplay(&g_sContext);
        test = AD7993_read_blocking(i2c_bus);
        p1 = ((uint8_t *) (test->readBuf))[0];
        p2 = ((uint8_t *) (test->readBuf))[1];
        r[0] = ((p1 & 0x0F) << 8) | p2;

        p1 = ((uint8_t *) (test->readBuf))[2];
        p2 = ((uint8_t *) (test->readBuf))[3];
        r[1] = ((p1 & 0x0F) << 8) | p2;

        p1 = ((uint8_t *) (test->readBuf))[4];
        p2 = ((uint8_t *) (test->readBuf))[5];
        r[2] = ((p1 & 0x0F) << 8) | p2;

        p1 = ((uint8_t *) (test->readBuf))[6];
        p2 = ((uint8_t *) (test->readBuf))[7];
        r[3] = ((p1 & 0x0F) << 8) | p2;

        cx[0] = snprintf ( str0, 20, "Vin 1 -- %d", r[0]);
        cx[1] = snprintf ( str1, 20, "Vin 2 -- %d", r[1]);
        cx[2] = snprintf ( str2, 20, "Vin 3 -- %d", r[2]);
        cx[3] = snprintf ( str3, 20, "Vin 4 -- %d", r[3]);

        logger.print((int8_t *) str0, cx[0]);
        state_tracker.update(6, (int8_t *) str1, cx[1], (int8_t *) str2, cx[2], (int8_t *) str3, cx[3]);
        vTaskDelay(pdMS_TO_TICKS(800));
    }
}
