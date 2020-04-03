/*
 * net_thread.h
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 */


#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "pthread.h"
#include "configs/Board.h"

#include <third_party/fatfs/ff.h>
#include <third_party/fatfs/diskio.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDFatFS.h>

#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/driverlib/gpio.h>

#include <ti/drivers/Board.h>
#include <ti/drivers/gpio/GPIOCC32XX.h>
#include <ti/devices/cc32xx/inc/hw_apps_rcm.h>
#include <ti/devices/cc32xx/inc/hw_ocp_shared.h>

#include "tasks/FGtask.h"
#include "drivers/LCD/FG_graphics.h"

#define SL_TASK_STACK_SIZE 2048 // copied from TI's tcpecho example
#define SPAWN_TASK_PRIORITY 9 // also copied from TI's tcpecho example


#ifndef TASKS_LCD_TASK_LCD_TASK_H_
#define TASKS_LCD_TASK_LCD_TASK_H_

#ifdef _cplusplus
#define
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
void lcd_task(void * par);


#ifdef _cplusplus
#define
extern }
#endif


#endif /* TASKS_NET_TASK_NET_TASK_H_ */
