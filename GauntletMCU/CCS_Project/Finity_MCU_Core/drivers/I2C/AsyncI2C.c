/*
 * AsyncI2C.c
 *
 *  Created on: Apr 21, 2020
 *      Author: halitosisman
 */


#include "AsyncI2C.h"


void Async_I2C_callback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus) {

    if (transaction->arg == NULL) { // transaction->arg should always be a pointer to the Async handle
        while (1) {

        }
    }

    // Add completed I2C transaction to queue for processing
    Async_I2C_Handle * async_handle = (Async_I2C_Handle *) transaction->arg;
    if (mq_send(async_handle->completed, (char *) &transaction, sizeof(&transaction), 0) == EAGAIN) {
        while (1) {

        }
    }
    sem_post(&(async_handle->completed_lock));
    sem_post(&(async_handle->i2c_lock));
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

    Async_I2C_Handle * device = malloc(sizeof(Async_I2C_Handle));

    I2C_Handle peripheral = I2C_open(0, &par);
    if(peripheral == NULL) {
        while(1) {

        }
    }

    // Initialize device parameters.
    device->i2c_handle = peripheral;
    sem_init(&(device->i2c_lock), 0, 0);
    sem_post(&(device->i2c_lock));
    sem_init(&(device->pending_lock), 0, 0);
    sem_init(&(device->completed_lock), 0, 0);
    device->i2c_params = malloc(sizeof(I2C_Params));
    *(device->i2c_params) = par;
    device->i2c_timeout_ms = timeout;
    struct mq_attr i2cqueue;
    i2cqueue.mq_maxmsg = TRANSACTION_QUEUE_SIZE;
    i2cqueue.mq_msgsize = sizeof(I2C_Transaction *);
    device->pending = mq_open("pending", O_CREAT | O_NONBLOCK, 0, &i2cqueue);
    device->completed = mq_open("completed", O_CREAT | O_NONBLOCK, 0, &i2cqueue);

    return device;
}

void Async_I2C_close(Async_I2C_Handle* handle)
{
    I2C_close(handle->i2c_handle);
    free(handle->i2c_params);
    free(handle);
}

bool Async_I2C_enqueue(Async_I2C_Handle* handle, I2C_Transaction* transaction)
{
    mq_send(handle->pending, (char *) &transaction, sizeof(&transaction), 0);
    sem_post(&(handle->pending_lock));
    return true;
}


void Async_I2C_process(Async_I2C_Handle* handle)
{
    int_fast16_t i2c_status;
    I2C_Transaction * pending[1];

    // If I2C transactions are pending, the I2C callback releases this lock. Otherwise, the lock is immediately
    // relinquished.
    if (sem_trywait(&(handle->i2c_lock)) == 0) {
        if (sem_trywait(&(handle->pending_lock)) == 0) {
            if (mq_receive(handle->pending, (char *) pending, sizeof(I2C_Transaction *), 0) > 0) {
                // important, pending should have the Async_I2C_Handle as an arg
                i2c_status = I2C_transfer(handle->i2c_handle, *pending);
                if (i2c_status != true) {
                    while(1) {

                    }
                }

            }
            else {
                sem_post(&(handle->i2c_lock));
                sem_post(&(handle->pending_lock));
            }
        }
        else {
            sem_post(&(handle->i2c_lock));
        }
    }
}

I2C_Transaction* Async_I2C_dequeue(Async_I2C_Handle* handle)
{
    I2C_Transaction * completed[1];

    if (sem_trywait(&(handle->completed_lock)) == 0) {
        if (mq_receive(handle->completed, (char *) completed, sizeof(I2C_Transaction *), 0) > 0) {
            return *completed;
        }
        else{
            sem_post(&(handle->completed_lock));
            return NULL;
        }
    }
    else {
        return NULL;
    }
}
