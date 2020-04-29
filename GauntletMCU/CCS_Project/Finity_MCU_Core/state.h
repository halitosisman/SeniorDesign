/*
 * i2c_event.h
 *
 *  Created on: Apr 24, 2020
 *      Author: halitosisman
 */

#ifndef TASKS_I2C_TASK_STATE_H_
#define TASKS_I2C_TASK_STATE_H_


#include <string.h>
#include <stdint.h>
#include "tasks/net_task/net_types.h"
#include "util/uart_term.h"
#include "util/error_manifest.h"


#define KILL_DEVICE "kill"
#define KILL_DEVICE_LEN 4


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Nav_Up = 0,
    Nav_Down,
    Nav_Right,
    Nav_Left
} Nav;

typedef enum {
    Device_Null = 0,
    Device_Light,
    Device_Motor,
    Device_Accel,
    Device_Temp,
    Device_System,
    Device_Command = Device_System,
    State_Count,
    Device_Count = State_Count
} State_List;

typedef struct _FG_State {
    State_List device_type;
    struct Command * selected_command;
    struct Device * selected_device;
} FG_State;

extern FG_State outgoing_state;
extern FG_State FG_user_state;

extern void init_FG_state();
extern bool update_FG_state(Nav i);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_I2C_TASK_STATE_H_ */
