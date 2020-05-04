/*
 * net_types.h
 *
 *  Created on: Apr 25, 2020
 *      Author: halitosisman
 */

#ifndef TASKS_NET_TASK_NET_TYPES_H_
#define TASKS_NET_TASK_NET_TYPES_H_


#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>

//*****************************************************************************
//                          LOCAL DEFINES
//*****************************************************************************
/* enables secured client                                                    */
//#define SECURE_CLIENT

/* enables client authentication by the server                               */
#define CLNT_USR_PWD

#define CLIENT_INIT_STATE        (0x01)
#define MQTT_INIT_STATE          (0x04)

#define APPLICATION_VERSION      "1.1.1"
#define APPLICATION_NAME         "MQTT client"

#define SLNET_IF_WIFI_PRIO       (5)

/* Operate Lib in MQTT 3.1 mode.                                             */
#define MQTT_3_1_1               false
#define MQTT_3_1                 true

#define WILL_TOPIC               "Client"
#define WILL_MSG                 "Client Stopped"
#define WILL_QOS                 MQTT_QOS_2
#define WILL_RETAIN              false

/* Defining Broker IP address and port Number                                */
//#define SERVER_ADDRESS           "messagesight.demos.ibm.com"
#define SERVER_ADDRESS           "tailor.cloudmqtt.com"
#define SERVER_IP_ADDRESS        "34.199.33.81"
#define PORT_NUMBER              17632
#define SECURED_PORT_NUMBER      27632
#define LOOPBACK_PORT            37632

/* Clean session flag                                                        */
#define CLEAN_SESSION            true

/* Retain Flag. Used in publish message.                                     */
#define RETAIN_ENABLE            1

/* Defining Number of subscription topics                                    */
#define SUBSCRIPTION_TOPIC_COUNT 2

/* Defining Subscription Topic Values                                        */
#define SUBSCRIPTION_TOPIC0      "status/#"
#define SUBSCRIPTION_TOPIC1      "new/#"

/* Spawn task priority and Task and Thread Stack Size                        */
#define TASKSTACKSIZE            2048
#define RXTASKSIZE               4096
#define MQTTTHREADSIZE           2048
#define SPAWN_TASK_PRIORITY      9


/* secured client requires time configuration, in order to verify server     */
/* certificate validity (date).                                              */

/* Day of month (DD format) range 1-31                                       */
#define DAY                      1
/* Month (MM format) in the range of 1-12                                    */
#define MONTH                    5
/* Year (YYYY format)                                                        */
#define YEAR                     2017
/* Hours in the range of 0-23                                                */
#define HOUR                     12
/* Minutes in the range of 0-59                                              */
#define MINUTES                  33
/* Seconds in the range of 0-59                                              */
#define SEC                      21

/* Number of files used for secure connection                                */
#define CLIENT_NUM_SECURE_FILES  1

/* Expiration value for the timer that is being used to toggle the Led.      */
#define TIMER_EXPIRATION_VALUE   100 * 1000000



#ifdef __cplusplus
extern "C" {
#endif


// Basically the C version of a base class for the other devices
struct Device {
    struct Device* next;
    struct Device* prev;
    char name[20];
    int name_len;
    char ID[9];
    float status;
};


struct Light {
    struct Light* next;
    struct Light* prev;
    char name[20];
    int name_len;
    char ID[9];
    float status;
    char pub_topic[23];
    bool dimmable;
};

struct Motor {
    struct Motor* next;
    struct Motor* prev;
    char name[20];
    int name_len;
    char ID[9];
    float status;
    char pub_topic[23];
    bool setable;
    bool reversable;
};

struct Accel {
    struct Accel* next;
    struct Accel* prev;
    char name[20];
    int name_len;
    char ID[9];
    float status;
    char pub_topic[23];
    bool setable;
};

struct Temp {
    struct Temp* next;
    struct Temp* prev;
    char name[20];
    int name_len;
    char ID[9];
    float status;
    char pub_topic[23];
    bool setable;
};

struct Device_List {
    struct Light* Light;
    int Light_len;
    struct Motor* Motor;
    int Motor_len;
    struct Accel* Accel;
    int Accel_len;
    struct Temp* Temp;
    int Temp_len;
};

struct Command {
    struct Command* next;
    struct Command* prev;
    char name[20];
    int name_len;
    char ID[5];
    //To publish command
    char command[10];
    int command_len;
};

struct Command_List {
    // L__
    struct Command* L;
    int L_len;
    // LD_
    struct Command* LD;
    int LD_len;

    // M__
    struct Command* M;
    int M_len;
    // MR_
    struct Command* MR;
    int MR_len;
    // M_S
    struct Command* MS;
    int MS_len;
    // MRS
    struct Command* MRS;
    int MRS_len;

    // A__
    struct Command* A;
    int A_len;
    // AF_
    struct Command* AF;
    int AF_len;

    // T__
    struct Command* T;
    int T_len;
    // TF_
    struct Command* TF;
    int TF_len;

    // S
    struct Command* S;
    int S_len;
};

extern pthread_mutex_t list_sync;
extern mqd_t g_PBQueue;
extern struct Device_List device_list;
extern struct Command_List command_list;


#ifdef __cplusplus
}
#endif

#endif /* TASKS_NET_TASK_NET_TYPES_H_ */