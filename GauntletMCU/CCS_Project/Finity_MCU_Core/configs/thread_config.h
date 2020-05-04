/*
 * thread_config.h
 *
 *  Created on: Feb 3, 2020
 *      Author: halitosisman
 */



#ifndef CONFIGS_THREAD_CONFIG_H_
#define CONFIGS_THREAD_CONFIG_H_


// Most of these values are arbitrary
#define MAILBOX_CNT 3
#define MAILBOX_SIZE 4

#define LETTER_PAYLOAD_MAX_SIZE 4  // bytes

#define NET_THREAD_ID 0
#define GUI_THREAD_ID 1
#define I2C_THREAD_ID 2
#define TASK_CNT 3

#define GUI_UPDATE_SIG 0b1

#define GUI_THREAD_STATE_TRACKER_STATE_CNT 3
#define GUI_THREAD_STATE_TRACKER_STATE_SIZE 15
#define GUI_THREAD_STATE_0_ID 0
#define GUI_THREAD_STATE_1_ID 1
#define GUI_THREAD_STATE_2_ID 2
#define GUI_THREAD_LOGGER_SIZE 40
#define GUI_THREAD_LOGGER_ID 0b1
#define GUI_THREAD_STATE_TRACKER_ID 0b10
#define GUI_THREAD_STACK_SIZE 1024
#define GUI_THREAD_PRIORITY 4

#define NET_TASK_MQTT_CLIENT_PRIORITY 6
#define NET_TASK_SPAWNER_PRIORITY 5
#define NET_TASK_SPAWNER_FINISHED_PRIORITY 2
#define NET_TASK_PRIORITY 7
#define NET_TASK_STACK_SIZE 1024  // arbitrary right now


#define I2C_THREAD_PRIORITY 3
#define I2C_THREAD_STACK_SIZE 256
#define I2C_THREAD_POLL_PERIOD_MS 5000
#define I2C_THREAD_ADC_THRESH 1860
#define I2C_THREAD_I2C_INT_FLAG 0b1
#define I2C_THREAD_ADC_TIMER_FLAG 0b10
#define I2C_THREAD_BUTTON_DELAY_MS 10

/*
    \effect Launches networking thread startup tasks

    \time After device power on or reset.
 */
#define WIFI_CONNECT_EVENT 0
#define WIFI_CONNECT_SUCCESS 0
#define WIFI_CONNECT_FAIL 1


#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif /* CONFIGS_THREAD_CONFIG_H_ */
