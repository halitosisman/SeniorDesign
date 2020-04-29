/*
 * net_thread.h
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 */


#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "pthread.h"
#include "configs/Board.h"

#include "tasks/FGtask.h"
#include "GUI/FG_GUI.h"

#include "state.h"

#define SL_TASK_STACK_SIZE 2048 // copied from TI's tcpecho example
#define SPAWN_TASK_PRIORITY 9 // also copied from TI's tcpecho example

#define BACKGROUND_UPDATE_PERIOD_MS 5000


#ifndef TASKS_LCD_TASK_LCD_TASK_H_
#define TASKS_LCD_TASK_LCD_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif


int32_t fatfs_getFatTime(void);


/*!
    \brief          lcd_task

    FreeRTOS task function for Finity Gauntlet graphical functions

    \param          par    -   Standard FreeRTOS task parameters.

    \return         void

    \note

*/
extern void gui_task(void * par);

#ifdef __cplusplus
}
#endif


#endif /* TASKS_NET_TASK_NET_TASK_H_ */
