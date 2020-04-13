/*
 * thread.cpp
 *
 *  Created on: Feb 4, 2020
 *      Author: halitosisman
 */



#include <tasks/FGtask.h>



void FGcreate_task(T_Params par)
{
    // The task creation function should change this to a non error value under
    //proper usage.
    BaseType_t status = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

    status = xTaskCreate(par.pvTaskCode, par.pcName, par.usStackDepth, par.pvParameters, par.uxPriority,
                         par.pxCreatedTask);

    // TODO Implement error logging
}
