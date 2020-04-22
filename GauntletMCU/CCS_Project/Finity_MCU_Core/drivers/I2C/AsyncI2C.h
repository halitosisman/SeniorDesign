/*
 * FG_I2C.h
 *
 *  Created on: Apr 21, 2020
 *      Author: halitosisman
 */

#ifndef DRIVERS_I2C_ASYNCI2C_H_
#define DRIVERS_I2C_ASYNCI2C_H_

#include <ti/drivers/I2C.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"


#define TRANSACTION_QUEUE_SIZE 20
#define TRANSACTION_ENQUEUE_MAX_TIME_MS 200
#define TRANSACTION_DEQUEUE_MAX_TIME_MS 400
#define ASYNC_I2C_TIMEOUT_MS 400

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _Async_I2C_Handle {
    I2C_Params * i2c_params;
    I2C_Handle i2c_handle;
    uint32_t i2c_timeout_ms;
    SemaphoreHandle_t i2c_lock;
    QueueHandle_t pending;
    QueueHandle_t completed;
} Async_I2C_Handle;


Async_I2C_Handle * Async_I2C_open(I2C_BitRate bitRate, uint32_t timeout);
bool Async_I2C_enqueue(Async_I2C_Handle * handle, I2C_Transaction *transaction);
I2C_Transaction * Async_I2C_process(Async_I2C_Handle* handle);
void Async_I2C_close(Async_I2C_Handle * handle);


#ifdef __cplusplus
}
#endif


#endif /* DRIVERS_I2C_ASYNCI2C_H_ */
