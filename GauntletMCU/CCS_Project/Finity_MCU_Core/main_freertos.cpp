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

/* RTOS header files */
#include <tasks/gui_task/gui_task.h>
#include <tasks/i2c_task/i2c_task.h>
#include "FreeRTOS.h"
#include "task.h"

#include "tasks/FGTask.h"

#include "configs/Board.h"
#include "configs/thread_config.h"

/*
 *  ======== main ========
 */
int main(void)
{
    FGthread_arg_t thread_args;
    T_Params net_arg;
    T_Params i2c_arg;
    T_Params gui_arg;

    /* Call driver init functions */
    Board_init();

    // Initialize the thread communication data structures
    thread_args.mailroom[GUI_THREAD_ID] = xQueueCreate(1, sizeof(GUI_Letter));

    // Network Thread Initialization
    i2c_arg.pcName = static_cast<char *>("i2c");
    i2c_arg.pvParameters = &thread_args;
    i2c_arg.pvTaskCode = i2c_task;
    i2c_arg.pxCreatedTask = &(thread_args.tasks[I2C_THREAD_ID]);
    i2c_arg.usStackDepth = I2C_THREAD_STACK_SIZE;
    i2c_arg.uxPriority = I2C_THREAD_PRIORITY;
    FGcreate_task(i2c_arg);

    net_arg.pcName = static_cast<char *>("net");
    net_arg.pvParameters = &thread_args;
    net_arg.pvTaskCode = NULL;
    net_arg.pxCreatedTask = &(thread_args.tasks[NET_THREAD_ID]);
    net_arg.usStackDepth = NET_TASK_STACK_SIZE;
    net_arg.uxPriority = NET_TASK_SPAWNER_PRIORITY;
    FGcreate_task(net_arg);

    gui_arg.pcName = static_cast<char *>("gui");
    gui_arg.pvParameters = &thread_args;
    gui_arg.pvTaskCode = gui_task;
    gui_arg.pxCreatedTask = &(thread_args.tasks[GUI_THREAD_ID]);
    gui_arg.usStackDepth = GUI_THREAD_STACK_SIZE;
    gui_arg.uxPriority = 5;//GUI_THREAD_PRIORITY; TEMPORARILY 5 FOR TESTING
    FGcreate_task(gui_arg);

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
