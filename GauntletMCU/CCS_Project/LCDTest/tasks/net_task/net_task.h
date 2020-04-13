/*
 * net_thread.h
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 */


#include <stdint.h>
#include <string.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "pthread.h"
#include "configs/Board.h"
#include <ti/drivers/SPI.h>

#include "tasks/FGtask.h"

#include "configs/wifi_config.h"

#define SL_TASK_STACK_SIZE 2048 // copied from TI's tcpecho example
#define SPAWN_TASK_PRIORITY 9 // also copied from TI's tcpecho example


#ifndef TASKS_NET_TASK_NET_TASK_H_
#define TASKS_NET_TASK_NET_TASK_H_

#ifdef _cplusplus
#define
extern "C" {
#endif


extern QueueHandle_t sl_event_box; // because there's no other way for sl_events to signal net_task


/*!
    \brief          net_task

    FreeRTOS task function for Finity Gauntlet networking

    \param          par    -   Standard FreeRTOS task parameters.

    \return         void

    \note           All networking functions should be called exclusively in this thread because many of them are not
                    thread safe.

*/
void net_task(void * par);


#ifdef _cplusplus
#define
extern }
#endif


#endif /* TASKS_NET_TASK_NET_TASK_H_ */
