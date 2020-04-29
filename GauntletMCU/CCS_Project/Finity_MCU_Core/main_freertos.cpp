/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_freertos.c ========
 */


#include "string.h"

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <tasks/gui_task/gui_task.h>
#include <tasks/i2c_task/i2c_task.h>
#include <tasks/net_task/net_task.h>
#include "FreeRTOS.h"
#include "task.h"

#include "tasks/FGTask.h"

#include "configs/Board.h"
#include "configs/thread_config.h"

pthread_t thread;
pthread_t i2c_thread;
pthread_t gui_thread;
FGthread_arg_t thread_args;
/*
 *  ======== main ========
 */
int main(void)
{
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int retc;
    int detachState;

    /* Call driver init functions */
    Board_init();

    // Initialize the thread communication data structures
//    struct mq_attr i2cbox;
//    i2cbox.mq_maxmsg = 1;
 //   i2cbox.mq_msgsize = sizeof(char);
  //  thread_args.mailroom[I2C_THREAD_ID] = mq_open("i2cbox", O_CREAT, 0, &i2cbox);

//    struct mq_attr guibox;
//    i2cbox.mq_maxmsg = 1;
 //   i2cbox.mq_msgsize = sizeof(char);
 //   thread_args.mailroom[GUI_THREAD_ID] = mq_open("guibox", O_CREAT, 0, &guibox);

//    struct mq_attr netbox;
 //   i2cbox.mq_maxmsg = 10;
 //   i2cbox.mq_msgsize = sizeof(struct msgQueue);
 //   thread_args.mailroom[NET_THREAD_ID] = mq_open("netbox", O_CREAT, 0, &netbox);

    //pthread_barrier_init(&(thread_args.sync[I2C_THREAD_ID]), )

    // Network Thread Initialization
   /* i2c_arg.pcName = static_cast<char *>("i2c");
    i2c_arg.pvParameters = &thread_args;
    i2c_arg.pvTaskCode = i2c_task;
    i2c_arg.pxCreatedTask = &(thread_args.tasks[I2C_THREAD_ID]);
    i2c_arg.usStackDepth = I2C_THREAD_STACK_SIZE;
    i2c_arg.uxPriority = I2C_THREAD_PRIORITY;
    FGcreate_task(i2c_arg);*/

    /* Set priority and stack size attributes */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = I2C_THREAD_PRIORITY; // 1

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, I2C_THREAD_STACK_SIZE * 4);
    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }

    retc = pthread_create(&i2c_thread, &pAttrs, (void * (*) (void *))i2c_task, &thread_args);
    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }



    pthread_attr_init(&pAttrs);
    priParam.sched_priority = NET_TASK_SPAWNER_PRIORITY; // 1

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, 4096);
    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }

    retc = pthread_create(&thread, &pAttrs, (void * (*) (void *))net_task, &thread_args);
    if(retc != 0)
    {
        while(1)
        {
            ;
        }
    }
    /*
    net_arg.pcName = static_cast<char *>("net");
    net_arg.pvParameters = &thread_args;
    net_arg.pvTaskCode = net_task;
    net_arg.pxCreatedTask = &(thread_args.tasks[NET_THREAD_ID]);
    net_arg.usStackDepth = NET_TASK_STACK_SIZE;
    net_arg.uxPriority = NET_TASK_SPAWNER_PRIORITY;
    FGcreate_task(net_arg);
     */
    /* gui_arg.pcName = static_cast<char *>("gui");
    gui_arg.pvParameters = &thread_args;
    gui_arg.pvTaskCode = gui_task;
    gui_arg.pxCreatedTask = &(thread_args.tasks[GUI_THREAD_ID]);
    gui_arg.usStackDepth = GUI_THREAD_STACK_SIZE;
    gui_arg.uxPriority = GUI_THREAD_PRIORITY;
    FGcreate_task(gui_arg);*/

    /* Set priority and stack size attributes */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = GUI_THREAD_PRIORITY; // 1

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(retc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while(1)
        {
            ;
        }
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, GUI_THREAD_STACK_SIZE * 4);
    if(retc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while(1)
        {
            ;
        }
    }

    retc = pthread_create(&gui_thread, &pAttrs, (void * (*) (void *))gui_task, &thread_args);
    if(retc != 0)
    {
        /* pthread_create() failed */
        while(1)
        {
            ;
        }
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();


    return (0);
}


#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}

#ifdef __cplusplus
}
#endif
