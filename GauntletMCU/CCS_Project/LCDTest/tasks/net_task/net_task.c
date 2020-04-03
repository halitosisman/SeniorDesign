/*
 * net_task.c
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 *
 *
 *      Much of the initialization code here was copied from the Simplelink CC32xx SDK tcpecho example
 */


#include "net_task.h"


QueueHandle_t sl_event_box;


static void Connect(void)
{
    SlWlanSecParams_t   secParams = {0};
    int16_t ret = 0;
    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;
    ret = sl_WlanConnect((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    if (ret)
    {
        while (1);
    }
}


static void net_init() {
    sl_event_box = xQueueCreate(MAILBOX_SIZE, sizeof(Letter_t));

    /*
        Getting an educated guess as to how sl_Task works took me a super long time so I am including an explanation
        here of sl_task and why I am copying TI's posix style thread creation for sl_Task instead of using a native
        FreeRTOS method.

        TI passes a pthread_Obj into sl_Task as a parameter. The TI documentation says nothing about sl_Task requiring
        parameters of any kind and its source file is not available. My attempts to launch sl_Task as a native FreeRTOS
        task with the same priority and stack size that TI assigned in its example resulted in an error state. To the
        best of my knowledge, said error state was FreeRTOS' response to a task function exiting. Given that the lack of
        documentation on sl_Task effectively makes it a black box, I am going to treat it as one by retaining TI's posix
         based code.
     */

    int16_t role;
    Letter_t cnct_letter;
    BaseType_t cnct_status;
    int32_t             status = 0;
    pthread_attr_t      pAttrs_spawn;
    struct sched_param  priParam;
    pthread_t spawn_thread = (pthread_t)NULL;

    /* Start the SimpleLink Host */
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = SPAWN_TASK_PRIORITY;
    status = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    status |= pthread_attr_setstacksize(&pAttrs_spawn, SL_TASK_STACK_SIZE);

    status = pthread_create(&spawn_thread, &pAttrs_spawn, sl_Task, NULL);
    if(status)
    {
        // TODO add stuff here once error logging system is online
    }

    role = sl_Start(NULL, NULL, NULL);
    if (role < 0) {
        while (1);
    }


    status = sl_WlanSetMode(ROLE_STA);
    if (status) {
        while (1); // TODO Error Handlers
    }

    status = sl_NetAppSet(SL_NETAPP_DEVICE_ID, SL_NETAPP_DEVICE_URN, strlen(DEVICE_NAME), DEVICE_NAME);
    if (status) {
        while (1);
    }

    status = sl_Stop(0);
    if (status) {
        while (1);
    }

    role = sl_Start(NULL, NULL, NULL);
    if (role != ROLE_STA) {
        while (1);
    }


    Connect();

    // Note that the timer for this queue receive timeout does not stop when paused in debug mode
    cnct_status = xQueueReceive(sl_event_box, &cnct_letter, pdMS_TO_TICKS(CONNECT_TIMEOUT_MS));
    if (cnct_status != pdPASS || cnct_letter.sig == WIFI_CONNECT_EVENT && cnct_letter.payload.integer ==
            WIFI_CONNECT_FAIL) {
        while (1);
    }

    // The end of this demo
    while (1);

}


void net_task(void * par) {
    net_init();
}
