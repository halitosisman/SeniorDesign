/*
 * ADC7993.h
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#ifndef DRIVERS_ADC_ADC7993_H_
#define DRIVERS_ADC_ADC7993_H_

#include <ti/drivers/I2C.h>


#define ADC7993_1_ADR 0b0100011

#define ADC7993_CH_CNT 4

#define AD7993_ALRT_STATUS_ADDR 0b0001
#define AD7993_CONF_ADDR 0b0010
#define AD7993_CYCLE_TIMER_ADDR 0b0011
#define AD7993_CH_CONFIG_BASE_ADDR 0b0100

#define AD7993_CONF_CH4_MSK 0b10000000
#define AD7993_CONF_CH3_MSK 0b01000000
#define AD7993_CONF_CH2_MSK 0b00100000
#define AD7993_CONF_CH1_MSK 0b00010000

#define AD7993_CONV_1234_ADR (AD7993_CONF_CH4_MSK | AD7993_CONF_CH3_MSK | AD7993_CONF_CH2_MSK | AD7993_CONF_CH1_MSK)

#define AD7993_CONF_FLTR_MSK 0b1000

#define AD7993_CONF_BUSY_ALERT_DISABLE_MSK 0b0000
#define AD7993_CONF_BUSY_MSK 0b0010
#define AD7993_CONF_ALERT_MSK 0b0100
#define AD7993_CONF_ALERT_RESET_MSK 0b0110

#define AD7993_CONF_BUSY_ALERT_ACTIVE_HIGH_MSK 0b1
#define AD7993_CONF_BUSY_ALERT_ACTIVE_LOW_MSK 0b0

#define AD7993_CYCLE_TIMER_OFF 0

#define AD7993_CONF_SIZE 2 // TODO dunno quite how big to make this yet
#define AD7993_CONV_READ_SIZE 8
#define AD7993_CONV_WRITE_SIZE 1
#define AD7993_CH_CONF_SIZE 3

#define FINITY_GAUNTLET_AD7993_CONF (AD7993_CONF_CH4_MSK | AD7993_CONF_CH3_MSK | AD7993_CONF_CH2_MSK | \
                                        AD7993_CONF_CH1_MSK | AD7993_CONF_FLTR_MSK |  AD7993_CONF_ALERT_MSK | \
                                        AD7993_CONF_BUSY_ALERT_ACTIVE_LOW_MSK)
#define FINITY_GAUNTLET_AD7993_HYSTERESIS 8 // units of LSB
#define FINITY_GAUNTLET_AD7993_DATA_HIGH 0xE8A // about 3V
#define FINITY_GAUNTLET_AD7993_DATA_LOW 0x174 // about 0.3V
#define FINITY_GAUNTLET_AD7993_I2C_TIMEOUT_TICKS 10000000


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _AD7993_Config {
    I2C_Params * i2c_params;
    uint32_t i2c_timeout_ms;

    uint8_t config;
    uint16_t data_high[ADC7993_CH_CNT];
    uint16_t data_low[ADC7993_CH_CNT];
    uint16_t hysteresis[ADC7993_CH_CNT];
    uint8_t cycle_timer;
} AD7993_Config;


extern void AD7993_i2c_callback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus);

I2C_Handle AD7993_init(AD7993_Config AD7993_Handle);
void AD7993_read_nonblocking(I2C_Handle device, AD7993_Config AD7993_Handle);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_ADC_ADC7993_H_ */
