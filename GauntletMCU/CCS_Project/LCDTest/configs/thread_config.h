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
#define DISPLAY_THREAD_ID 1
#define I2C_THREAD_ID 2

#define NET_TASK_PRIORITY 5
#define NET_TASK_STACK_SIZE 256  // arbitrary right now

/*
    \effect Launches networking thread startup tasks

    \time After device power on or reset.
 */
#define WIFI_CONNECT_EVENT 0
#define WIFI_CONNECT_SUCCESS 0
#define WIFI_CONNECT_FAIL 1


#ifdef __cplusplus
#define
extern "C" {
#endif



#ifdef __cplusplus
#define
}
#endif


#endif /* CONFIGS_THREAD_CONFIG_H_ */
