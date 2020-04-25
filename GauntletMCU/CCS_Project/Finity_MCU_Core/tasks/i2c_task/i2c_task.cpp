/*
 * i2c_task.c
 *
 *  Created on: Apr 20, 2020
 *      Author: halitosisman
 */



#include "i2c_task.h"


static TaskHandle_t i2c_task_handle;
static TaskHandle_t gui_task_handle;

void i2c_int_callback(uint_least8_t index) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(i2c_task_handle, I2C_THREAD_I2C_INT_FLAG, eSetBits, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void adc_timer_callback(TimerHandle_t xTimer) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(i2c_task_handle, I2C_THREAD_ADC_TIMER_FLAG, eSetBits, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}



void i2c_task(void * par) {
    i2c_task_handle = ((FGthread_arg_t *) par)->tasks[I2C_THREAD_ID];
    gui_task_handle = ((FGthread_arg_t *) par)->tasks[GUI_THREAD_ID];
    GUI_Letter gui_letter;
    uint32_t notify_flags = 0;
    TimerHandle_t adc_delayer = xTimerCreate("adc_delayer", pdMS_TO_TICKS(50), pdFALSE, ADC_TIMER_ID,
                                             adc_timer_callback);
    Logger logger = Logger();
    State_Tracker state_tracker = State_Tracker();

    AD7993_Config FG_AD7993_Handle = // TODO Move to task files
    {
     .config = FINITY_GAUNTLET_AD7993_CONF,
     .cycle_timer = AD7993_CYCLE_TIMER_OFF,
     .data_high[0] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_high[1] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_high[2] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_high[3] = FINITY_GAUNTLET_AD7993_DATA_HIGH,
     .data_low[0] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .data_low[1] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .data_low[2] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .data_low[3] = FINITY_GAUNTLET_AD7993_DATA_LOW,
     .hysteresis[0] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[1] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[2] = FINITY_GAUNTLET_AD7993_HYSTERESIS,
     .hysteresis[3] = FINITY_GAUNTLET_AD7993_HYSTERESIS
    };

    bool adc_read_active = false;
    Async_I2C_Handle * AD7993 = AD7993_init(&FG_AD7993_Handle);
    I2C_Transaction * adc_read;

    init_FG_state();
    FG_GUI_init();
    logger.init();
    state_tracker.init();

    while(1) {
        notify_flags = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(I2C_THREAD_POLL_PERIOD_MS));

        if (notify_flags & I2C_THREAD_I2C_INT_FLAG) {
            if (notify_flags & I2C_THREAD_ADC_TIMER_FLAG) {
                AD7993_start_read(AD7993);
                adc_read_active = true;
            }
            else {
                xTimerStart(adc_delayer, 0);
            }
        }
        else if (notify_flags & I2C_THREAD_ADC_TIMER_FLAG) {
             AD7993_start_read(AD7993);
            adc_read_active = true;
        }

        // Insert other I2C checks here

        vTaskDelay(pdMS_TO_TICKS(1)); // Wait for I2C transactions to finish

        struct msgQueue netMsg;
        uint16_t r = 0;
        if (adc_read_active) {
            adc_read = AD7993_end_read(AD7993);
            adc_read_active = false;
            for (uint8_t i = 0; i < ADC7993_CH_CNT * 2; i += 2) {
                r = AD7993_convert_result(static_cast<uint8_t *>(adc_read->readBuf)[i],
                                          static_cast<uint8_t *>(adc_read->readBuf)[i + 1]);
                if (r > I2C_THREAD_ADC_THRESH) {
                    if (update_FG_state(static_cast<Nav>(i / 2))) {
                        gui_letter.cmd_issued = true;
                    }
                    else {
                        gui_letter.cmd_issued = false;
                    }
                    netMsg.event = USR_CMD;
                    netMsg.msgPtr = NULL;
                    netMsg.topLen = 0;
                    mq_send(g_PBQueue, (char *) &netMsg, sizeof(netMsg), 0);
                    gui_letter.state = FG_user_state;
                    vTaskDelay(pdMS_TO_TICKS(10)); // Wait for the networking thread to update
                    gui_update(gui_letter, static_cast<FGthread_arg_t *>(par)->mailroom);
                }
            }
        }
    }
}

void gui_update(GUI_Letter state, QueueHandle_t* mailroom)
{
    xQueueOverwrite(mailroom[GUI_THREAD_ID], &state);
}
