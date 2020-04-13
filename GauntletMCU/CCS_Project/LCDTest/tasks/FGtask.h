/*
 * thread.h
 *
 *  Created on: Feb 3, 2020
 *      Author: halitosisman
 */

#ifndef CONFIGS_THREAD_H_
#define CONFIGS_THREAD_H_


#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "configs/thread_config.h"


#ifdef __cplusplus
#define
extern "C" {
#endif


typedef struct _Letter {
    uint8_t sig;
    union {
        int integer;
        uint8_t char_array[LETTER_PAYLOAD_MAX_SIZE];
    } payload;
} Letter_t;


/*!
 *  @brief      Register of all threads and their shared data structures. Don't modify this after the scheduler has been
 *  started for now.
 *
 */
typedef struct _FGthread_arg {
    QueueHandle_t mailroom[MAILBOX_CNT];

} FGthread_arg_t;

/*!
 *  @brief      Globally visible mailbox for all FreeRTOS threads.
 *
 */


/*!
 *  @brief      Required parameters for Finity Gauntlet thread creation.
 */
typedef struct _T_Params {

    /*

    The following lines quote the original documentation at
    https://www.freertos.org/wp-content/uploads/2018/07/FreeRTOS_Reference_Manual_V10.0.0.pdf
    or
    https://www.freertos.org/wp-content/uploads/2018/07/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf

     *
     *
    Tasks are simply C functions that never exit and, as such, are normally
    implemented as an infinite loop. The pvTaskCode parameter is simply a
    pointer to the function (in effect, just the function name) that implements the
    task.
     */
    TaskFunction_t pvTaskCode;

    /*
    A descriptive name for the task. This is mainly used to facilitate debugging,
    but can also be used in a call to xTaskGetHandle() to obtain a task handle.

    The application-defined constant configMAX_TASK_NAME_LEN defines the
    maximum length of the name in characters – including the NULL terminator.
    Supplying a string longer than this maximum will result in the string being
    35
    silently truncated.
     */
    char * pcName;

    /*
    Each task has its own unique stack that is allocated by the kernel to the task
    when the task is created. The usStackDepth value tells the kernel how large
    to make the stack.

    The value specifies the number of words the stack can hold, not the number
    of bytes. For example, on an architecture with a 4 byte stack width, if
    usStackDepth is passed in as 100, then 400 bytes of stack space will be
    allocated (100 * 4 bytes). The stack depth multiplied by the stack width must
    not exceed the maximum value that can be contained in a variable of type
    size_t.

    The size of the stack used by the idle task is defined by the application
    defined constant configMINIMAL_STACK_SIZE. The value assigned to this
    constant in the demo application provided for the chosen microcontroller
    architecture is the minimum recommended for any task on that architecture.
    If your task uses a lot of stack space, then you must assign a larger value.
     */
    unsigned short usStackDepth;

    /*
    Task functions accept a parameter of type ‘pointer to void’ ( void* ). The
    value assigned to pvParameters will be the value passed into the task.
    This parameter has the type ‘pointer to void’ to allow the task parameter to
    effectively, and indirectly by means of casting, receive a parameter of any
    type. For example, integer types can be passed into a task function by
    casting the integer to a void pointer at the point the task is created, then by
    casting the void pointer parameter back to an integer in the task function
    definition itself.
     */
    void *pvParameters;

    /*
    Defines the priority at which the task will execute. Priorities can be assigned
    from 0, which is the lowest priority, to (configMAX_PRIORITIES – 1), which
    is the highest priority.

    configMAX_PRIORITIES is a user defined constant. If
    configUSE_PORT_OPTIMISED_TASK_SELECTION is set to 0 then there is
    no upper limit to the number of priorities that can be available (other than the
    limit of the data types used and the RAM available in your microcontroller),
    but it is advised to use the lowest number of priorities required, to avoid
    36
    wasting RAM.

    Passing a uxPriority value above (configMAX_PRIORITIES – 1) will result in
    the priority assigned to the task being capped silently to the maximum
    legitimate value.
     */
    UBaseType_t uxPriority;

    /*
    pxCreatedTask can be used to pass out a handle to the task being created.
    This handle can then be used to reference the task in API calls that, for
    example, change the task priority or delete the task.

    If your application has no use for the task handle, then pxCreatedTask can
    be set to NULL.
     */
    TaskHandle_t *pxCreatedTask;
} T_Params;



/*!
    \brief          FGcreate_task

    Initializes a FreeRTOS thread in the context of the Finity Gauntlet project.

    \param          T_Params    -   struct containing the FreeRTOS xTaskCreate parameters.

    \return         void

    \note           This function should be used instead of xTaskCreate in the Finity Gauntlet project.

*/
void FGcreate_task(T_Params par);


#ifdef __cplusplus
#define
}
#endif



#endif /* CONFIGS_THREAD_H_ */
