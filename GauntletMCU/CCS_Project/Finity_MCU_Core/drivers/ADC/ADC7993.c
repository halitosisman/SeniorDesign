/*
 * ADC7993.c
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */


#include "ADC7993.h"


static I2C_Params i2c_params =
{
 .bitRate = I2C_400kHz,
 .custom = NULL,
 .transferCallbackFxn = AD7993_i2c_callback,
 .transferMode = I2C_MODE_BLOCKING
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



I2C_Handle AD7993_init(AD7993_Config AD7993_Handle) {

    uint8_t conf_buf[] = {AD7993_CONF_ADDR, AD7993_Handle.config};

    I2C_Transaction config_ADconf =
    {
     .arg = NULL,
     .writeCount = sizeof(conf_buf),
     .writeBuf = conf_buf,
     .readCount = 0,
     .readBuf = NULL,
     .slaveAddress = ADC7993_1_ADR
    };

    uint8_t write_cycle_timer_buf[] = {AD7993_CYCLE_TIMER_ADDR, AD7993_Handle.cycle_timer};
    I2C_Transaction config_cycle_timer =
    {
     .arg = NULL,
     .readBuf = NULL,
     .readCount = 0,
     .slaveAddress = ADC7993_1_ADR,
     .writeBuf = write_cycle_timer_buf,
     .writeCount = sizeof(write_cycle_timer_buf)
    };

    uint8_t ch_config_buf[AD7993_CH_CONF_SIZE];
    I2C_Transaction ch_config =
    {
     .arg = NULL,
     .readBuf = NULL,
     .readCount = 0,
     .slaveAddress = ADC7993_1_ADR,
     .writeBuf = ch_config_buf,
     .writeCount = sizeof(ch_config_buf)
    };


    I2C_Handle AD7993 = I2C_open(0, AD7993_Handle.i2c_params);
    if(AD7993 == NULL) {
        while(1) {

        }
    }

    I2C_transferTimeout(AD7993, &config_ADconf, AD7993_Handle.i2c_timeout_ms);
    I2C_transferTimeout(AD7993, &config_cycle_timer, AD7993_Handle.i2c_timeout_ms);

    // Initialize data_high, data_low and hysteresis registers for all channels
    uint8_t ch_addr = AD7993_CH_CONFIG_BASE_ADDR;
    for(int i = 0; i < ADC7993_CH_CNT; i++) {
        ch_config_buf[0] = ch_addr;
        ch_config_buf[1] = FG_AD7993_Handle.data_low[i] >> 8;
        ch_config_buf[2] = FG_AD7993_Handle.data_low[i] & 0xFF;
        I2C_transferTimeout(AD7993, &ch_config, AD7993_Handle.i2c_timeout_ms);
        ch_addr += 1;

        ch_config_buf[0] = ch_addr;
        ch_config_buf[1] = FG_AD7993_Handle.data_high[i] >> 8;
        ch_config_buf[2] = FG_AD7993_Handle.data_high[i] & 0xFF;
        I2C_transferTimeout(AD7993, &ch_config, AD7993_Handle.i2c_timeout_ms);
        ch_addr += 1;

        ch_config_buf[0] = ch_addr;
        ch_config_buf[1] = FG_AD7993_Handle.hysteresis[i] >> 8;
        ch_config_buf[2] = FG_AD7993_Handle.hysteresis[i] & 0xFF;
        I2C_transferTimeout(AD7993, &ch_config, AD7993_Handle.i2c_timeout_ms);
        ch_addr += 1;
    }

    return AD7993;
}


static uint8_t write_conv_buf[] = {AD7993_CONV_1234_ADR};
static uint8_t read_conv_buf[AD7993_CONV_READ_SIZE];
static I2C_Transaction read_conv =
{
 .arg = NULL,
 .writeBuf = write_conv_buf,
 .writeCount = sizeof(write_conv_buf),
 .readBuf = read_conv_buf,
 .readCount = sizeof(read_conv_buf),
 .slaveAddress = ADC7993_1_ADR
};


void AD7993_read_nonblocking(I2C_Handle device, AD7993_Config AD7993_Handle) {
    I2C_transferTimeout(device, &read_conv, AD7993_Handle.i2c_timeout_ms);
}









