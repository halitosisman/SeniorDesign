/*
 * ADC7993.c
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */


#include "ADC7993.h"


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


Async_I2C_Handle * AD7993_init(AD7993_Config * AD7993_config) {

    uint8_t conf_buf[] = {AD7993_CONF_ADDR, AD7993_config->config};

    Async_I2C_Handle * AD7993 = Async_I2C_open(I2C_400kHz, ASYNC_I2C_TIMEOUT_MS);
    if(AD7993 == NULL) {
        while(1) {

        }
    }

    read_conv.arg = AD7993;

    I2C_Transaction config_ADconf =
    {
     .arg = AD7993,
     .writeCount = sizeof(conf_buf),
     .writeBuf = conf_buf,
     .readCount = 0,
     .readBuf = NULL,
     .slaveAddress = ADC7993_1_ADR
    };

    uint8_t write_cycle_timer_buf[] = {AD7993_CYCLE_TIMER_ADDR, AD7993_config->cycle_timer};
    I2C_Transaction config_cycle_timer =
    {
     .arg = AD7993,
     .readBuf = NULL,
     .readCount = 0,
     .slaveAddress = ADC7993_1_ADR,
     .writeBuf = write_cycle_timer_buf,
     .writeCount = sizeof(write_cycle_timer_buf)
    };

    uint8_t ch_config_buf[AD7993_CH_CONF_SIZE];
    I2C_Transaction ch_config =
    {
     .arg = AD7993,
     .readBuf = NULL,
     .readCount = 0,
     .slaveAddress = ADC7993_1_ADR,
     .writeBuf = ch_config_buf,
     .writeCount = sizeof(ch_config_buf)
    };

    //I2C_transferTimeout(AD7993, &config_ADconf, AD7993_Handle.i2c_timeout_ms);
    Async_I2C_enqueue(AD7993, &config_ADconf);
    Async_I2C_enqueue(AD7993, &config_cycle_timer);

    // Initialize data_high, data_low and hysteresis registers for all channels
    uint8_t ch_addr = AD7993_CH_CONFIG_BASE_ADDR;
    for(int i = 0; i < ADC7993_CH_CNT; i++) {
        ch_config_buf[0] = ch_addr;
        ch_config_buf[1] = AD7993_config->data_low[i] >> 8;
        ch_config_buf[2] = AD7993_config->data_low[i] & 0xFF;
        Async_I2C_enqueue(AD7993, &ch_config);
        ch_addr += 1;

        ch_config_buf[0] = ch_addr;
        ch_config_buf[1] = AD7993_config->data_high[i] >> 8;
        ch_config_buf[2] = AD7993_config->data_high[i] & 0xFF;
        Async_I2C_enqueue(AD7993, &ch_config);
        ch_addr += 1;

        ch_config_buf[0] = ch_addr;
        ch_config_buf[1] = AD7993_config->hysteresis[i] >> 8;
        ch_config_buf[2] = AD7993_config->hysteresis[i] & 0xFF;
        Async_I2C_enqueue(AD7993, &ch_config);
        ch_addr += 1;
    }

    int8_t counter = 0;
    while (counter < 2 + 3 * ADC7993_CH_CNT) {
        if (Async_I2C_process(AD7993) != NULL) {
            counter++;
        }
    }

    return AD7993;
}


I2C_Transaction * AD7993_read_blocking(Async_I2C_Handle * AD7993_Handle) {
    I2C_Transaction * result = NULL;

    Async_I2C_enqueue(AD7993_Handle, &read_conv);
    while (result == NULL) {
        result = Async_I2C_process(AD7993_Handle);
    }
    return result;
}

void AD7993_start_read(Async_I2C_Handle* AD7993_Handle)
{
    Async_I2C_enqueue(AD7993_Handle, &read_conv);
}

I2C_Transaction* AD7993_end_read(Async_I2C_Handle* AD7993_Handle)
{
    I2C_Transaction * result = Async_I2C_process(AD7993_Handle);
    while (result == NULL) {
        result = Async_I2C_process(AD7993_Handle);
    }
    return result;
}

uint16_t AD7993_convert_result(uint32_t MSB, uint32_t LSB)
{
    return ((MSB & 0x0F) << 8) | LSB;
}
