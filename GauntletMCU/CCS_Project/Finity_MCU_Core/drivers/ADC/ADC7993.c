/*
 * ADC7993.c
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */


#include "ADC7993.h"


uint8_t write_conv_buf[] = {AD7993_CONV_1234_ADR};
uint8_t read_conv_buf[AD7993_CONV_READ_SIZE];
I2C_Transaction read_conv =
{
 .arg = NULL,
 .writeBuf = write_conv_buf,
 .writeCount = sizeof(write_conv_buf),
 .readBuf = read_conv_buf,
 .readCount = sizeof(read_conv_buf),
 .slaveAddress = ADC7993_1_ADR
};

static uint8_t write_conf_buf[] = {0b0111};
static uint8_t read_conf_buf[2];
static I2C_Transaction read_conf =
{
 .arg = NULL,
 .writeBuf = write_conf_buf,
 .writeCount = sizeof(write_conf_buf),
 .readBuf = read_conf_buf,
 .readCount = sizeof(read_conf_buf),
 .slaveAddress = ADC7993_1_ADR
};

static uint8_t write_reset_alert_buf[] = {AD7993_CONF_ADDR, };
static I2C_Transaction reset_alrt =
{
 .arg = NULL,
 .writeBuf = write_reset_alert_buf,
 .writeCount = sizeof(write_reset_alert_buf),
 .readBuf = NULL,
 .readCount = 0,
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
    read_conf.arg = AD7993;
    reset_alrt.arg = AD7993;

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

    uint8_t ch_config_buf[ADC7993_CH_CNT][3][AD7993_CH_CONF_SIZE];
    I2C_Transaction ch_config[ADC7993_CH_CNT][3];

    // Initialize data_high, data_low and hysteresis registers for all channels
    uint8_t ch_addr = AD7993_CH_CONFIG_BASE_ADDR;
    for(int i = 0; i < ADC7993_CH_CNT; i++) {
        for(int j = 0; j < 3; j++) {
            ch_config[i][j].arg = AD7993;
            ch_config[i][j].readBuf = NULL;
            ch_config[i][j].readCount = 0;
            ch_config[i][j].slaveAddress = ADC7993_1_ADR;
            ch_config[i][j].writeBuf = &(ch_config_buf[i][j]);
            ch_config[i][j].writeCount = AD7993_CH_CONF_SIZE;
        }
        ch_config_buf[i][0][0] = ch_addr;
        ch_config_buf[i][0][1] = AD7993_config->data_low[i] >> 8;
        ch_config_buf[i][0][2] = AD7993_config->data_low[i] & 0xFF;
        Async_I2C_enqueue(AD7993, &(ch_config[i][0]));
        ch_addr += 1;

        ch_config_buf[i][1][0] = ch_addr;
        ch_config_buf[i][1][1] = AD7993_config->data_high[i] >> 8;
        ch_config_buf[i][1][2] = AD7993_config->data_high[i] & 0xFF;
        Async_I2C_enqueue(AD7993, &(ch_config[i][1]));
        ch_addr += 1;

        ch_config_buf[i][2][0] = ch_addr;
        ch_config_buf[i][2][1] = AD7993_config->hysteresis[i] >> 8;
        ch_config_buf[i][2][2] = AD7993_config->hysteresis[i] & 0xFF;
        Async_I2C_enqueue(AD7993, &(ch_config[i][2]));
        ch_addr += 1;
    }
    Async_I2C_enqueue(AD7993, &config_ADconf);
    Async_I2C_enqueue(AD7993, &config_cycle_timer);

    int8_t counter = 0;
    while (counter < 2 + 3 * ADC7993_CH_CNT) {
        Async_I2C_process(AD7993);
        if (Async_I2C_dequeue(AD7993) != NULL) {
            Async_I2C_process(AD7993);
            counter++;
        }
    }

    return AD7993;
}


I2C_Transaction * AD7993_read_config(Async_I2C_Handle * AD7993_Handle, uint8_t addr) {
    I2C_Transaction * result = NULL;

    ((uint8_t *)(read_conf.writeBuf))[0] = addr;
    Async_I2C_enqueue(AD7993_Handle, &read_conf);
    while (result == NULL) {
        Async_I2C_process(AD7993_Handle);
        result = Async_I2C_dequeue(AD7993_Handle);
    }
    return result;
}

I2C_Transaction * AD7993_read_conv(Async_I2C_Handle * AD7993_Handle) {
    I2C_Transaction * result = NULL;

    Async_I2C_enqueue(AD7993_Handle, &read_conv);
    while (result == NULL) {
        Async_I2C_process(AD7993_Handle);
        result = Async_I2C_dequeue(AD7993_Handle);
    }
    return result;
}

void AD7993_start_read(Async_I2C_Handle* AD7993_Handle)
{
    Async_I2C_enqueue(AD7993_Handle, &read_conv);
}

I2C_Transaction* AD7993_end_read(Async_I2C_Handle* AD7993_Handle)
{
    I2C_Transaction * result = Async_I2C_dequeue(AD7993_Handle);
    while (result == NULL) {
        Async_I2C_process(AD7993_Handle);
        result = Async_I2C_dequeue(AD7993_Handle);
    }
    return result;
}

uint16_t AD7993_convert_result(uint32_t MSB, uint32_t LSB)
{
    return ((MSB & 0x0F) << 8) | LSB;
}

I2C_Transaction* AD7993_get_config(Async_I2C_Handle* AD7993_Handle)
{
    I2C_Transaction * result = NULL;

    Async_I2C_enqueue(AD7993_Handle, &read_conf);
    while (result == NULL) {
        Async_I2C_process(AD7993_Handle);
        result = Async_I2C_dequeue(AD7993_Handle);
    }
    return result;
}

I2C_Transaction* AD7993_reset_alert(Async_I2C_Handle* AD7993_Handle)
{
    I2C_Transaction * result = NULL;

    Async_I2C_enqueue(AD7993_Handle, &reset_alrt);
    while (result == NULL) {
        Async_I2C_process(AD7993_Handle);
        result = Async_I2C_dequeue(AD7993_Handle);
    }
    return result;
}
