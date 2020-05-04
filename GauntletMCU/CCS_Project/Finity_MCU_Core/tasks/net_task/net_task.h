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


//*****************************************************************************
/* Defines for the MQTT publish and subscribe roots for each of the different 
devices to process MQTT messages and to send MQTT messages */
//Topics
#define LIGHT_COMMAND_PUB_TOPIC "command/light/"
#define LIGHT_COMMAND_PUB_TOPIC_LEN 14
#define LIGHT_STATUS_SUB_TOPIC "status/light/"
#define LIGHT_NEW_SUB_TOPIC "new/light/"

#define MOTOR_COMMAND_PUB_TOPIC "command/motor/"
#define MOTOR_COMMAND_PUB_TOPIC_LEN 14
#define MOTOR_STATUS_SUB_TOPIC "status/motor/"
#define MOTOR_NEW_SUB_TOPIC "new/motor/"

#define ACCEL_COMMAND_PUB_TOPIC "command/accel/"
#define ACCEL_COMMAND_PUB_TOPIC_LEN 14
#define ACCEL_STATUS_SUB_TOPIC "status/accel/"
#define ACCEL_NEW_SUB_TOPIC "new/accel/"

#define TEMP_COMMAND_PUB_TOPIC "command/tempr/"
#define TEMP_COMMAND_PUB_TOPIC_LEN 14
#define TEMP_STATUS_SUB_TOPIC "status/tempr/"
#define TEMP_NEW_SUB_TOPIC "new/tempr/"

#define SYSTEM_PUB_TOPIC "system/"
#define SYSTEM_PUB_TOPIC_LEN 7

/* Defines for the commands in the command list of each device type and 
command length need for MQTT publish */
// Light command list
#define LIGHT_GET_STATUS "stat_gt"
#define LIGHT_GET_STATUS_LEN 7

#define LIGHT_SET_ON "set_on"
#define LIGHT_SET_ON_LEN 6

#define LIGHT_SET_OFF "set_off"
#define LIGHT_SET_OFF_LEN 7

#define LIGHT_RESET_STATUS "stat_rs"
#define LIGHT_RESET_STATUS_LEN 7

#define LIGHT_SAVE_STATUS "stat_sv"
#define LIGHT_SAVE_STATUS_LEN 7

#define LIGHT_PAUSE "pause"
#define LIGHT_PAUSE_LEN 5

#define LIGHT_RESUME "resume"
#define LIGHT_RESUME_LEN 6

#define LIGHT_DIM "dim"
#define LIGHT_DIM_LEN 3

#define LIGHT_BRIGHTEN "bright"
#define LIGHT_BRIGHTEN_LEN 6

#define LIGHT_SET_BRIGHTNESS "set_br"
#define LIGHT_SET_BRIGHTNESS_LEN 6

#define LIGHT_CONNECTION_CONFIRM "confirm"
#define LIGHT_CONNECTION_CONFIRM_LEN 7

// Motor command list
#define MOTOR_GET_STATUS "stat_gt"
#define MOTOR_GET_STATUS_LEN 7

#define MOTOR_SET_FORWARD "set_fr"
#define MOTOR_SET_FORWARD_LEN 6

#define MOTOR_SET_OFF "set_off"
#define MOTOR_SET_OFF_LEN 7

#define MOTOR_SET_BACKWARDS "set_bk"
#define MOTOR_SET_BACKWARDS_LEN 6

#define MOTOR_RESET_STATUS "stat_rs"
#define MOTOR_RESET_STATUS_LEN 7

#define MOTOR_SAVE_STATUS "stat_sv"
#define MOTOR_SAVE_STATUS_LEN 7

#define MOTOR_PAUSE "pause"
#define MOTOR_PAUSE_LEN 6

#define MOTOR_RESUME "resume"
#define MOTOR_RESUME_LEN 6

#define MOTOR_SLOW "slow"
#define MOTOR_SLOW_LEN 4

#define MOTOR_SPEED "spd"
#define MOTOR_SPEED_LEN 3

#define MOTOR_SET_SPEED "set_spd"
#define MOTOR_SET_SPEED_LEN 7

#define MOTOR_CONNECTION_CONFIRM "confirm"
#define MOTOR_CONNECTION_CONFIRM_LEN 7


// Accel command list
#define ACCEL_GET_STATUS "stat_gt"
#define ACCEL_GET_STATUS_LEN 7

#define ACCEL_SET_ON "set_on"
#define ACCEL_SET_ON_LEN 6

#define ACCEL_SET_OFF "set_off"
#define ACCEL_SET_OFF_LEN 7

#define ACCEL_SET_FREQ "set_fq"
#define ACCEL_SET_FREQ_LEN 6

#define ACCEL_CONNECTION_CONFIRM "confirm"
#define ACCEL_CONNECTION_CONFIRM_LEN 7


// Temp command list
#define TEMP_GET_STATUS "stat_gt"
#define TEMP_GET_STATUS_LEN 7

#define TEMP_SET_ON "set_on"
#define TEMP_SET_ON_LEN 6

#define TEMP_SET_OFF "set_off"
#define TEMP_SET_OFF_LEN 7

#define TEMP_SET_FREQ "set_fq"
#define TEMP_SET_FREQ_LEN 6

#define TEMP_CONNECTION_CONFIRM "confirm"
#define TEMP_CONNECTION_CONFIRM_LEN 7

// System and Syst command list
#define SYSTEM_GET_STATUS "stat_gt"
#define SYSTEM_GET_STATUS_LEN 7

#define SYSTEM_RESET_STATUS "stat_rs"
#define SYSTEM_RESET_STATUS_LEN 7

#define SYSTEM_UPDATE "update"
#define SYSTEM_UPDATE_LEN 6

#define SYST_PUB_TOPIC "syst/"
#define SYST_PUB_TOPIC_LEN 5

#define KILL_DEVICE "kill"
#define KILL_DEVICE_LEN 4

#define CLEAR_COMMAND " "
#define CLEAR_COMMAND_LEN 1
//*****************************************************************************


#define SL_TASK_STACK_SIZE 2048 // copied from TI's tcpecho example
#define SPAWN_TASK_PRIORITY 9 // also copied from TI's tcpecho example


#ifndef TASKS_NET_TASK_NET_TASK_H_
#define TASKS_NET_TASK_NET_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_t thread;



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
void clean_device_list();


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