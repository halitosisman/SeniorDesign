/*
 * AD7993.h
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */


/*!****************************************************************************
 *  @file       AD7993.h
 *  @brief      I2C ADC7993 Driver for the Finity Gauntlet project
 *
 *  This driver provides functionality for setting the configuration registers
 *  and reading values from the Analog Devices AD7993 ADC IC.
 *
 *  The I2C driver as defined under ti/drivers/I2C.h must be initialized for
 *  this driver to work. Additionally, the AD7993 driver is hardcoded to
 *  initialize the I2C driver in nonblocking mode. Only one instance of this
 *  driver should be open at any given time.
 *
 *  ***************************************************************************/


#ifndef DRIVERS_ADC_ADC7993_H_
#define DRIVERS_ADC_ADC7993_H_

#include "drivers/I2C/AsyncI2C.h"


// The particular device for this project had the 1 marking and had the AS pin tied to use this address
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
#define AD7993_CYCLE_TIMER_T256 0b100

#define AD7993_CONF_SIZE 2
#define AD7993_CONV_READ_SIZE 8
#define AD7993_CONV_WRITE_SIZE 1
#define AD7993_CH_CONF_SIZE 3

#ifdef __cplusplus
extern "C" {
#endif


/*!
 *  @brief  Defines the values for the settings registers.
 *
 *  @sa AD7993_init()
 */
typedef struct _AD7993_Config {
    /*!
     *  value to write to the AD7993 configuration register.
     */
    uint8_t config;

    /*!
     *  The upper limit of the conversion value needed to set the alert pin.
     */
    uint16_t data_high[ADC7993_CH_CNT];

    /*!
     *  The lower limit of the conversion value needed to set the alert pin.
     */
    uint16_t data_low[ADC7993_CH_CNT];

    /*!
     *  The number of least significant bits that need to change away from the data high and data low values for the
     *  alert to reset.
     */
    uint16_t hysteresis[ADC7993_CH_CNT];

    /*!
     *  If set to a nonzero value, determines the frequency of conversion result register updates.
     */
    uint8_t cycle_timer;
} AD7993_Config;


/*!
 *  @brief  The callback function for the AD7993 alert pin.
 *
 *  This function is called whenever on the falling edge of the alert line. This is intended to be a value for
 *  I2C_CallbackFxn from I2C.h.
 *
 *  @warning  The callback function is called from an interrupt context.
 *
 *  @param[out]  handle    #I2C_Handle used with the initial call to
 *  I2C_transfer()
 *
 *  @param[out]  transaction    Pointer to the #I2C_Transaction structure used
 *  with the initial call to I2C_transfer(). This structure also contains the
 *  custom argument specified by @p transaction.arg.
 *
 *  @param[out]  transferStatus    Boolean indicating if the I2C transaction
 *  was successful. If @p true, the transaction was successful. If @p false,
 *  the transaction failed.
 */
extern void AD7993_i2c_callback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus);

/*!
 *  @brief  The initialization function for the AD7993
 *
 *  @pre    I2C_init() has been called.
 *
 *  @param[in]  AD7993_config    Initialized configuration structure corresponding to the AD7993 register structure.
 *
 *  @return  Async_I2C_Handle * used for subsequent AD7993 driver functions on success. Will probably crash the whole
 *  system on a failure.
 */
Async_I2C_Handle * AD7993_init(AD7993_Config * AD7993_config);

/*!
 *  @brief  Debugging utility function to ensure AD7993 registers have been set correctly.
 *
 *  @pre    AD7993_init() has been called.
 *
 *  @param[in]  AD7993_Handle    Handle to an initialized instance of the Async AD7993 driver.
 *
 *  @param[in]  addr    AD7993 address to write to.
 *
 *  @return  I2C_Transaction * on success. Will probably crash the whole system on a failure.
 */
I2C_Transaction * AD7993_read_config(Async_I2C_Handle * AD7993_Handle, uint8_t addr);

/*!
 *  @brief  Reads the contents of the AD7993 configuration register.
 *
 *  @pre    AD7993_init() has been called.
 *
 *  @param[in]  AD7993_Handle    Handle to an initialized instance of the Async AD7993 driver.
 *
 *  @return  I2C_Transaction * on success. Will probably crash the whole system on a failure.
 */
I2C_Transaction * AD7993_get_config(Async_I2C_Handle * AD7993_Handle);

/*!
 *  @brief  Reads the contents of the AD7993 conversion registers.
 *
 *  @pre    AD7993_init() has been called.
 *
 *  @param[in]  AD7993_Handle    Handle to an initialized instance of the Async_I2C driver.
 *
 *  @return  I2C_Transaction * on success. Will probably crash the whole system on a failure.
 */
I2C_Transaction * AD7993_read_conv(Async_I2C_Handle * AD7993_Handle);

/*!
 *  @brief  Resets the alert pin on the AD7993
 *
 *  @pre    AD7993_init() has been called.
 *
 *  @param[in]  AD7993_Handle    Handle to an initialized instance of the Async AD7993 driver.
 *
 *  @return  I2C_Transaction * on success. Will probably crash the whole system on a failure.
 */
I2C_Transaction * AD7993_reset_alert(Async_I2C_Handle * AD7993_Handle);

/*!
 *  @brief  Adds an I2C conversion result register read transaction to the Async_I2C driver queue
 *
 *  @pre    AD7993_init() has been called.
 *
 *  @param[in]  AD7993_Handle    Handle to an initialized instance of the Async AD7993 driver.
 *
 *  @return  I2C_Transaction * on success. Will probably crash the whole system on a failure.
 */
void AD7993_start_read(Async_I2C_Handle * AD7993_Handle);

/*!
 *  @brief  Adds an I2C conversion result register read transaction to the Async_I2C driver queue
 *
 *  @pre    AD7993_init() has been called.
 *
 *  @param[in]  AD7993_Handle    Handle to an initialized instance of the Async AD7993 driver.
 *
 *  @return  I2C_Transaction * on success. Will probably crash the whole system on a failure.
 */
I2C_Transaction * AD7993_end_read(Async_I2C_Handle * AD7993_Handle);

/*!
 *  @brief Concatenates the two parameters into a single value.
 *
 *  @param[in]  MSB    The most significant byte of a conversion result.
 *
 *  @param[in]  LSB    The least significant byte of a conversion result.
 *
 *  @return  uint16_t with MSB = MSB and LSB = LSB
 */
uint16_t AD7993_convert_result(uint32_t MSB, uint32_t LSB);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_ADC_ADC7993_H_ */
