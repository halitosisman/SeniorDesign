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
    GPIO_enableInt(GPIO_I2C_Int);

    init_FG_state();
    gui_update(static_cast<FGthread_arg_t *>(par)->tasks[GUI_THREAD_ID]);

    while(1) {
        notify_flags = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(I2C_THREAD_POLL_PERIOD_MS));

        if (notify_flags & I2C_THREAD_ADC_TIMER_FLAG) { // If the delay timer for the ADC has finished
            AD7993_start_read(AD7993);
            adc_read_active = true;
        }
        else if (notify_flags & I2C_THREAD_I2C_INT_FLAG) { // If the pushbutton has been pressed
            xTimerStart(adc_delayer, 0); // Note that this resets the timer if it is active
        }

        // Insert other I2C checks here if we decide to add them

        vTaskDelay(pdMS_TO_TICKS(1)); // Wait for I2C transactions to finish

        struct msgQueue netMsg;
        uint16_t r = 0;
        if (adc_read_active) { // If there's a finished ADC transaction in need of processing
            // Take ADC transaction off the I2C queue and reset pending flag
            adc_read = AD7993_end_read(AD7993);
            adc_read_active = false;

            // Look for a high ADC channel. If multiple channels are high this acts based on the one with the highest
            // index.
            for (uint8_t i = 0; i < ADC7993_CH_CNT * 2; i += 2) {
                r = AD7993_convert_result(static_cast<uint8_t *>(adc_read->readBuf)[i],
                                          static_cast<uint8_t *>(adc_read->readBuf)[i + 1]);
                if (r > I2C_THREAD_ADC_THRESH) {
                    if (update_FG_state(static_cast<Nav>(i / 2))) { // Update the top level state machine.
                        gui_letter.cmd_issued = true;
                    }
                    else {
                        gui_letter.cmd_issued = false;
                    }
                    // Signal the MQTT thread
                    netMsg.event = USR_CMD;
                    netMsg.msgPtr = NULL;
                    netMsg.topLen = 0;
                    mq_send(g_PBQueue, (char *) &netMsg, sizeof(netMsg), 0);

                    gui_letter.state = FG_user_state;
                    vTaskDelay(pdMS_TO_TICKS(10)); // Wait for the networking thread to update
                    gui_update(static_cast<FGthread_arg_t *>(par)->tasks[GUI_THREAD_ID]);
                }
            }
        }
    }
}


void gui_update(TaskHandle_t gui_task) {
    xTaskNotify(gui_task, GUI_UPDATE_SIG, eSetBits);
}

