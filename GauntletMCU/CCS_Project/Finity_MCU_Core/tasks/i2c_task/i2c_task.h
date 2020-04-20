/*
 * i2c_task.h
 *
 *  Created on: Apr 20, 2020
 *      Author: halitosisman
 */

#ifndef TASKS_I2C_TASK_I2C_TASK_H_
#define TASKS_I2C_TASK_I2C_TASK_H_


#include "tasks/FGtask.h"
#include "drivers/ADC/ADC7993.h"


#ifdef __cplusplus
extern "C" {
#endif

extern QueueHandle_t i2c_event_box; // because there's no other way for sl_events to signal net_task

void AD7993_i2c_callback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus) {

}


/*!
    \brief          net_task

    FreeRTOS task function for Finity Gauntlet networking

    \param          par    -   Standard FreeRTOS task parameters.

    \return         void

    \note           All networking functions should be called exclusively in this thread because many of them are not
                    thread safe.

*/
extern void i2c_task(void * par);

#ifdef __cplusplus
}
#endif



#endif /* TASKS_I2C_TASK_I2C_TASK_H_ */
