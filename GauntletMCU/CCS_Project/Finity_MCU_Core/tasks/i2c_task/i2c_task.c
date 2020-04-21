/*
 * i2c_task.c
 *
 *  Created on: Apr 20, 2020
 *      Author: halitosisman
 */



#include "i2c_task.h"

static I2C_Params i2c_params =
{
 .bitRate = I2C_400kHz,
 .custom = NULL,
 .transferCallbackFxn = AD7993_i2c_callback,
 .transferMode = I2C_MODE_CALLBACK
};


static AD7993_Config FG_AD7993_Handle = // TODO Move to task file
{
 .i2c_params = &i2c_params,
 .i2c_timeout_ms = FINITY_GAUNTLET_AD7993_I2C_TIMEOUT_TICKS,
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

int testxyzf;
void AD7993_i2c_callback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus) {
    testxyzf += 1;
}


void i2c_task(void * par) {
    I2C_Handle AD = AD7993_init(FG_AD7993_Handle); // TODO Add semaphores to prevent queue buildup
    AD7993_read_nonblocking(AD, FG_AD7993_Handle);
    while(1) {

    }
}
