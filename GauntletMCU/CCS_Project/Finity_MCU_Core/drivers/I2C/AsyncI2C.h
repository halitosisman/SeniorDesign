/*
 * FG_I2C.h
 *
 *  Created on: Apr 21, 2020
 *      Author: halitosisman
 */

#ifndef DRIVERS_I2C_ASYNCI2C_H_
#define DRIVERS_I2C_ASYNCI2C_H_

/*!****************************************************************************
 *  @file       AsyncI2C.h
 *  @brief      Asynchronous I2C transaction driver
 *
 *  Allows for asynchronous I2C Transactions
 *
 *  The TI drivers seem to only allow one queued asynchronous I2C transaction.
 *  This driver allows more to be queued. I ended up using these drivers in a
 *  blocking fashion. Oh well.
 *
 *  This driver maintains a pending queue and a completed queue for I2C
 *  transactions. The Async_I2C_enqueue() function adds an I2C transaction *
 *  to the pending queue. The Process function moves an item from the pending
 *  queue to the I2C bus if the bus is available, and returns immediately
 *  whether or not a new transaction can be executed. An internally defined
 *  I2C callback unlocks the I2C bus and adds items to the completed queue on
 *  completion of an ongoing I2C transaction.
 *
 *  ***************************************************************************/

#include <ti/drivers/I2C.h>

#include "FreeRTOS.h"

#include <stdlib.h>

#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>
#include <errno.h>


#define TRANSACTION_QUEUE_SIZE 20
#define TRANSACTION_ENQUEUE_MAX_TIME_MS 200
#define TRANSACTION_DEQUEUE_MAX_TIME_MS 400
#define ASYNC_I2C_TIMEOUT_MS 400

#ifdef __cplusplus
extern "C" {
#endif


/*!
 *  @brief  Defines the values for the settings registers.
 *
 *  @sa AD7993_init()
 */
typedef struct _Async_I2C_Handle {
    /*!
     *  I2C device settings
     */
    I2C_Params * i2c_params;

    /*!
     *  I2C device handle
     */
    I2C_Handle i2c_handle;

    /*!
     *  Timeout for I2C transactions. Not used.
     */
    uint32_t i2c_timeout_ms;

    /*!
     *  Classic locking semaphore for the I2C peripheral
     */
    sem_t i2c_lock;

    /*!
     *  Lock for the pending I2C transaction queue. Future iterations to this code should probably get rid of this. It
     *  was only added as part of a bug search.
     */
    sem_t pending_lock;

    /*!
     *  Lock for the completed I2C transaction queue. Future iterations to this code should probably get rid of this. It
     *  was only added as part of a bug search.
     */
    sem_t completed_lock;

    /*!
     *  Queue for pending I2C transactions. Holds I2C_Transaction pointers.
     */
    mqd_t pending;

    /*!
     *  Queue for completed I2C transactions. Holds I2C_Transaction pointers.
     */
    mqd_t completed;
} Async_I2C_Handle;

/*!
 *  @brief  Initialization function for the Async_I2C driver.
 *
 *  @param[in]  bitRate    I2C bus speed.
 *
 *  @param[in]  timeout    Not used currently.
 *
 *  @return  Async_I2C_Handle *    Handle for an opened instance of the AsyncI2C driver.
 */
Async_I2C_Handle * Async_I2C_open(I2C_BitRate bitRate, uint32_t timeout);

/*!
 *  @brief  Adds an I2C transaction to the pending queue
 *
 *  @param[in]  Async_I2C_Handle *    Driver instance holding the specific pending queue
 *
 *  @param[in]  I2C_Transaction *    I2C transaction to enqueue
 *
 *  @return     bool    Always true currently
 */
bool Async_I2C_enqueue(Async_I2C_Handle * handle, I2C_Transaction *transaction);

/*!
 *  @brief  Handles pending I2C transactions.
 *
 *  If the I2C bus is available, lock it, remove an I2C_Transaction * from the pending queue, and execute it.
 *
 *  @param[in]  Async_I2C_Handle *    Driver instance holding the specific pending queue
 */
void Async_I2C_process(Async_I2C_Handle* handle);

/*!
 *  @brief  Returns a completed I2C transaction.
 *
 *  @param[in]  Async_I2C_Handle *    Driver instance holding the specific pending queue

 *  @return  I2C_Transaction *    The completed I2C transaction.
 */
I2C_Transaction * Async_I2C_dequeue(Async_I2C_Handle* handle);

/*!
 *  @brief   Frees memory associated with the I2C driver and shuts down the I2C bus.
 *
 *  @param[in]  Async_I2C_Handle *    Driver instance holding the specific pending queue.
 */
void Async_I2C_close(Async_I2C_Handle * handle);


#ifdef __cplusplus
}
#endif


#endif /* DRIVERS_I2C_ASYNCI2C_H_ */
