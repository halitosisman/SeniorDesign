/*
 * AsyncI2C.c
 *
 *  Created on: Apr 21, 2020
 *      Author: halitosisman
 */


#include "AsyncI2C.h"


void Async_I2C_callback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (transaction->arg == NULL) { // transaction->arg should always be a pointer to the Async handle
        while (1) {

        }
    }

    // Add completed I2C transaction to queue for processing
    Async_I2C_Handle * async_handle = (Async_I2C_Handle *) transaction->arg;
    if (xQueueSendToBackFromISR(async_handle->completed, &transaction, NULL) == errQUEUE_FULL) {
        while (1) { // this wasn't supposed to happen

        }
    }


    xSemaphoreGiveFromISR(async_handle->i2c_lock, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


// Note, this implementation isn't generalized to environments with multiple I2C peripherals
Async_I2C_Handle * Async_I2C_open(I2C_BitRate bitRate, uint32_t timeout)
{
    I2C_Params par =
    {
     .bitRate = bitRate,
     .custom = NULL,
     .transferCallbackFxn = Async_I2C_callback,
     .transferMode = I2C_MODE_CALLBACK
    };

    Async_I2C_Handle * device = pvPortMalloc(sizeof(Async_I2C_Handle));

    I2C_Handle peripheral = I2C_open(0, &par);
    if(peripheral == NULL) {
        while(1) {

        }
    }

    device->i2c_handle = peripheral;
    device->i2c_lock = xSemaphoreCreateBinary();
    xSemaphoreGive(device->i2c_lock);
    device->i2c_params = pvPortMalloc(sizeof(I2C_Params));
    *(device->i2c_params) = par;
    device->i2c_timeout_ms = timeout;
    device->pending = xQueueCreate(TRANSACTION_QUEUE_SIZE, sizeof(I2C_Transaction *));
    device->completed = xQueueCreate(TRANSACTION_QUEUE_SIZE, sizeof(I2C_Transaction *));
    return device;
}

void Async_I2C_close(Async_I2C_Handle* handle)
{
    I2C_close(handle->i2c_handle);
    vPortFree(handle->i2c_params);
    vPortFree(handle);
}

bool Async_I2C_enqueue(Async_I2C_Handle* handle, I2C_Transaction* transaction)
{
    xQueueSendToBack(handle->pending, &transaction, pdMS_TO_TICKS(TRANSACTION_ENQUEUE_MAX_TIME_MS));

    return true;
}


void Async_I2C_process(Async_I2C_Handle* handle)
{
    int_fast16_t i2c_status;
    I2C_Transaction * pending[1];

    // If I2C transactions are pending, the I2C callback releases this lock. Otherwise, the lock is immediately
    // relinquished.
    if (xSemaphoreTake(handle->i2c_lock, 0) == pdPASS) {
        if (xQueueReceive(handle->pending, pending, pdMS_TO_TICKS(TRANSACTION_DEQUEUE_MAX_TIME_MS)) == pdPASS) {
            // important, pending should have the Async_I2C_Handle as an arg
            i2c_status = I2C_transferTimeout(handle->i2c_handle, *pending, pdMS_TO_TICKS(ASYNC_I2C_TIMEOUT_MS));
            if (i2c_status != I2C_STATUS_SUCCESS) {
                while(1) {

                }
            }
        }
        // No I2C transactions pending, no need to lock the i2c peripheral
        else if (xSemaphoreGive(handle->i2c_lock) != pdPASS) {
            while(1) {

            }
        }

    }
}

I2C_Transaction* Async_I2C_dequeue(Async_I2C_Handle* handle)
{
    I2C_Transaction * completed[1];

    if (xQueueReceive(handle->completed, completed, pdMS_TO_TICKS(TRANSACTION_DEQUEUE_MAX_TIME_MS)) == pdPASS) {
        return *completed;
    }
    else{
        return NULL;
    }
}
