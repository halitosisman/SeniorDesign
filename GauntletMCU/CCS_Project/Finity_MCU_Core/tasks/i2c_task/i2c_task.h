/*
 * i2c_task.h
 *
 *  Created on: Apr 20, 2020
 *      Author: halitosisman
 */

#ifndef TASKS_I2C_TASK_I2C_TASK_H_
#define TASKS_I2C_TASK_I2C_TASK_H_


#include <ti/drivers/GPIO.h>
#include "tasks/FGtask.h"
#include "drivers/ADC/AD7993.h"
#include "tasks/net_task/net_task.h"
#include "configs/Board.h"
#include "state.h"


#define FINITY_GAUNTLET_AD7993_CONF (AD7993_CONF_CH4_MSK | AD7993_CONF_CH3_MSK | AD7993_CONF_CH2_MSK | \
                                        AD7993_CONF_CH1_MSK | AD7993_CONF_FLTR_MSK |  AD7993_CONF_ALERT_MSK | \
                                        AD7993_CONF_BUSY_ALERT_ACTIVE_LOW_MSK | AD7993_CONF_ALERT_RESET_MSK)
#define FINITY_GAUNTLET_AD7993_HYSTERESIS 4 // units of LSB
#define FINITY_GAUNTLET_AD7993_DATA_HIGH 0xFFC // about 3V
#define FINITY_GAUNTLET_AD7993_DATA_LOW 1860 // about 0.3V
#define FINITY_GAUNTLET_AD7993_I2C_TIMEOUT_TICKS 10000000


#ifdef __cplusplus
extern "C" {
#endif


/*!
    \brief          net_task

    FreeRTOS task function for Finity Gauntlet networking

    \param          par    -   Standard FreeRTOS task parameters.

    \return         void

    \note           All networking functions should be called exclusively in this thread because many of them are not
                    thread safe.

*/
void i2c_task(void * par);

void gui_update();

#ifdef __cplusplus
}
#endif



#endif /* TASKS_I2C_TASK_I2C_TASK_H_ */
