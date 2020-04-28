/*
 * net_thread.h
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 */


#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <semphr.h>
/* Standard includes                                                         */
#include <pthread.h>
#include <mqueue.h>
#include <time.h>
#include <unistd.h>

/* Common interface includes                                                 */
#include "network_if.h"
#include "util/uart_term.h"

#include "configs/Board.h"
#include <ti/drivers/SPI.h>

#include "tasks/FGtask.h"
#include "tasks/net_task/net_types.h"

#include "configs/wifi_config.h"

/* Application includes                                                      */
#include "client_cbs.h"


#define SL_TASK_STACK_SIZE 2048 // copied from TI's tcpecho example
#define SPAWN_TASK_PRIORITY 9 // also copied from TI's tcpecho example


#ifndef TASKS_NET_TASK_NET_TASK_H_
#define TASKS_NET_TASK_NET_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_t thread;


//*****************************************************************************


//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void DisplayBanner(char * AppName);
void * MqttClient(void *pvParameters);
void Mqtt_ClientStop(uint8_t disconnect);
void Mqtt_ServerStop();
void Mqtt_Stop();
void Mqtt_start();
int32_t Mqtt_IF_Connect();
int32_t MqttServer_start();
int32_t MqttClient_start();
int32_t MQTT_SendMsgToQueue(struct msgQueue *queueElement);


/*!
    \brief          net_task

    FreeRTOS task function for Finity Gauntlet networking

    \param          par    -   Standard FreeRTOS task parameters.

    \return         void

    \note           All networking functions should be called exclusively in this thread because many of them are not
                    thread safe.

*/
void net_task(void * par);


#ifdef __cplusplus
}
#endif


#endif /* TASKS_NET_TASK_NET_TASK_H_ */
