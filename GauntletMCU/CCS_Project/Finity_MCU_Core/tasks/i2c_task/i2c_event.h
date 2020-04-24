/*
 * i2c_event.h
 *
 *  Created on: Apr 24, 2020
 *      Author: halitosisman
 */

#ifndef TASKS_I2C_TASK_I2C_EVENT_H_
#define TASKS_I2C_TASK_I2C_EVENT_H_


#include "tasks/FGtask.h"


#ifdef __cplusplus
extern "C" {
#endif

enum Nav {
    Nav_Up = 0,
    Nav_Down,
    Nav_Right,
    Nav_Left
};

enum Nav_Depth {
    Nav_Type = 0,
    Nav_Cmd
};

enum Devices {
    Device_Null = 0,
    Device_Light,
    Device_Motor,
    Device_Accel,
    Device_Temp,
    Device_System,
    Device_Count
};

enum Commands {
    Null_Command = 0,
    Special_Command_Count,
    Light_General = Special_Command_Count,
    Light_Dimmable,
    Light_Command_Sentinel,
    Motor_General = Light_Command_Sentinel,
    Motor_Reversable,
    Motor_Settable,
    Motor_Reverse_Settable,
    Motor_Command_Sentinel,
    Temp_General = Motor_Command_Sentinel,
    Temp_Frequency,
    Temperature_Command_Sentinel,
    Accel_General = Temperature_Command_Sentinel,
    Accel_Frequency,
    Command_Count
};


typedef struct State {
    uint8_t nav_depth;

    Devices low;
    Devices high;
    Commands low;
    Commands high;
};


void update_FG_state(uint8_t i);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_I2C_TASK_I2C_EVENT_H_ */
