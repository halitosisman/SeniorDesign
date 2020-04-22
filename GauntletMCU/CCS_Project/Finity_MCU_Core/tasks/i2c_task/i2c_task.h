/*
 * i2c_task.h
 *
 *  Created on: Apr 20, 2020
 *      Author: halitosisman
 */

#ifndef TASKS_I2C_TASK_I2C_TASK_H_
#define TASKS_I2C_TASK_I2C_TASK_H_


#define I2C_MAX_BLOCK_TIME_MS 500


#include "tasks/FGtask.h"
#include "drivers/ADC/ADC7993.h"
#include "stdio.h"

#include "drivers/LCD/FG_graphics.h"
#include "GUI/FG_GUI.h" // TODO REMOVE ONCE SMAT IS DONE


#define ADC_TIMER_ID 0

#ifdef __cplusplus
extern "C" {
#endif

extern QueueHandle_t i2c_event_box; // because there's no other way for sl_events to signal net_task


/*!
    \brief          net_task

    FreeRTOS task function for Finity Gauntlet networking

    \param          par    -   Standard FreeRTOS task parameters.

    \return         void

    \note           All networking functions should be called exclusively in this thread because many of them are not
                    thread safe.

*/
void i2c_task(void * par);

void gui_update(GUI_State state, QueueHandle_t * mailroom);

#ifdef __cplusplus
}
#endif



#endif /* TASKS_I2C_TASK_I2C_TASK_H_ */
