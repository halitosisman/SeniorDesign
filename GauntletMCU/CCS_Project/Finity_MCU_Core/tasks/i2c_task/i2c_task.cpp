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
    int test = GPIO_read(GPIO_I2C_Int);
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
    uint32_t notify_flags = 0;
    TimerHandle_t adc_delayer = xTimerCreate("adc_delayer", pdMS_TO_TICKS(I2C_THREAD_BUTTON_DELAY_MS),
                                             pdFALSE, ADC_TIMER_ID, adc_timer_callback);

    AD7993_Config FG_AD7993_Handle = // TODO Move to task files
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

    bool adc_read_active = false;
    Async_I2C_Handle * AD7993 = AD7993_init(&FG_AD7993_Handle);
    I2C_Transaction * adc_read;
    GPIO_disableInt(GPIO_I2C_Int);
    GPIO_clearInt(GPIO_I2C_Int);
    GPIO_setConfig(GPIO_I2C_Int, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(GPIO_I2C_Int, i2c_int_callback);
    GPIO_enableInt(GPIO_I2C_Int);

    // For debugging
    adc_read = AD7993_read_config(AD7993, AD7993_ALRT_STATUS_ADDR);
    adc_read = AD7993_read_config(AD7993, AD7993_CONF_ADDR);
    adc_read = AD7993_read_config(AD7993, AD7993_CYCLE_TIMER_ADDR);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 1);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 2);

    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 3);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 4);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 5);

    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 6);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 7);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 8);

    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 9);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 10);
    adc_read = AD7993_read_config(AD7993, AD7993_CH_CONFIG_BASE_ADDR + 11);
    adc_read = AD7993_read_config(AD7993, AD7993_ALRT_STATUS_ADDR);

    init_FG_state();
    gui_update(static_cast<FGthread_arg_t *>(par)->tasks[GUI_THREAD_ID]);

    while(1) {
        notify_flags = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(portMAX_DELAY));

        if (notify_flags & I2C_THREAD_ADC_TIMER_FLAG) { // If the delay timer for the ADC has finished
            AD7993_start_read(AD7993);
            adc_read_active = true;
        }
        else if (notify_flags & I2C_THREAD_I2C_INT_FLAG) { // If the pushbutton has been pressed
            xTimerStart(adc_delayer, 0); // Note that this resets the timer if it is active
        }

        // Insert other I2C checks here if we decide to add them

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
                if (r < I2C_THREAD_ADC_THRESH) {
                    if (update_FG_state(static_cast<Nav>(i / 2))) { // Update the top level state machine.
                    }
                    else {
                    }
                    // Signal the MQTT thread
                    netMsg.event = USR_CMD;
                    netMsg.msgPtr = NULL;
                    netMsg.topLen = 0;
                    mq_send(g_PBQueue, (char *) &netMsg, sizeof(netMsg), 0);

                    vTaskDelay(pdMS_TO_TICKS(10)); // Wait for the networking thread to update
                    gui_update(static_cast<FGthread_arg_t *>(par)->tasks[GUI_THREAD_ID]);
                    break;
                }
            }
        }
    }
}


void gui_update(TaskHandle_t gui_task) {
    xTaskNotify(gui_task_handle, GUI_UPDATE_SIG, eSetBits);
}

