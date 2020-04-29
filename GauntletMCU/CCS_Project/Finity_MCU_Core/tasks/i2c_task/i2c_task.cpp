/*
 * i2c_task.c
 *
 *  Created on: Apr 20, 2020
 *      Author: halitosisman
 */



#include "i2c_task.h"

static mqd_t gui_mailroom;

static sem_t sync;
uint8_t test = 0;

void i2c_int_callback(uint_least8_t index) {
    //sem_post(&sync);
    test = 1;
}
/*
struct mq_attr i2cbox =
{
 .mq_maxmsg = 1,
 .mq_msgsize = sizeof(char)
};
struct mq_attr netbox =
{
 .mq_maxmsg = 1,
 .mq_msgsize = sizeof(struct msgQueue)
};
struct mq_attr guibox =
{
 .mq_maxmsg = 1,
 .mq_msgsize = sizeof(char)
};
*/
void i2c_task(void * par) {
    //((FGthread_arg_t *) par)->mailroom[I2C_THREAD_ID] = mq_open("i2cbox", O_CREAT, 0, &i2cbox);
    //((FGthread_arg_t *) par)->mailroom[GUI_THREAD_ID] = mq_open("guibox", O_CREAT, 0, &guibox);
   // ((FGthread_arg_t *) par)->mailroom[NET_THREAD_ID] = mq_open("netbox", O_CREAT, 0, &netbox);

    gui_mailroom = ((FGthread_arg_t *) par)->mailroom[GUI_THREAD_ID];

    sem_init(&sync, 0, 0);

    AD7993_Config FG_AD7993_Handle =
    {
     .config = FINITY_GAUNTLET_AD7993_CONF,
     .cycle_timer = AD7993_CYCLE_TIMER_T256,
     .data_high[0] = 0xFFC,
     .data_high[1] = 0xFFC,
     .data_high[2] = 0xFFC,
     .data_high[3] = 0xFFC,
     .data_low[0] = 1860,
     .data_low[1] = 1860,
     .data_low[2] = 1860,
     .data_low[3] = 1860,
     .hysteresis[0] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[1] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[2] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[3] = FINITY_GAUNTLET_AD7993_HYSTERESIS
    };

    Async_I2C_Handle * AD7993 = AD7993_init(&FG_AD7993_Handle);
    I2C_Transaction * adc_read;
    GPIO_disableInt(GPIO_I2C_Int);
    GPIO_clearInt(GPIO_I2C_Int);
    GPIO_setConfig(GPIO_I2C_Int, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(GPIO_I2C_Int, i2c_int_callback);
    GPIO_enableInt(GPIO_I2C_Int);

    init_FG_state();
    gui_update();

    while(1) {
       // mq_receive(i2c_mailroom, &notify_flags, sizeof(uint8_t), NULL);
        while (test == 0) {

        }
        test = 0;

        AD7993_start_read(AD7993);
        // Insert other I2C checks here if we decide to add them

        struct msgQueue netMsg;
        uint16_t r = 0;
        // Take ADC transaction off the I2C queue and reset pending flag
        adc_read = AD7993_end_read(AD7993);

        // Look for a high ADC channel. If multiple channels are high this acts based on the one with the highest
        // index.
        for (uint8_t i = 0; i < ADC7993_CH_CNT * 2; i += 2) {
            r = AD7993_convert_result(static_cast<uint8_t *>(adc_read->readBuf)[i],
                                      static_cast<uint8_t *>(adc_read->readBuf)[i + 1]);
            if (r < I2C_THREAD_ADC_THRESH) {
                pthread_mutex_lock(&list_sync);
                if (update_FG_state(static_cast<Nav>(i / 2))) { // Update the top level state machine.
                    pthread_mutex_unlock(&list_sync);
                    // Signal the MQTT thread
                    netMsg.event = USR_CMD;
                    netMsg.msgPtr = NULL;
                    netMsg.topLen = 0;
                    mq_send(g_PBQueue, (char *) &netMsg, sizeof(netMsg), 0);
                }
                else {
                    pthread_mutex_unlock(&list_sync);
                }

                usleep(10);
                gui_update();
                break;
            }
        }
    }
}


void gui_update() {
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);

    char msg = 0;
    mq_timedsend(gui_mailroom, &msg, sizeof(msg), 0, &ts);
}

