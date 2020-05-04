/*
 * i2c_event.h
 *
 *  Created on: Apr 24, 2020
 *      Author: halitosisman
 */

/*!****************************************************************************
 *  @file       state.h
 *  @brief      Handles the finite state machine for the Finity Gauntlet.
 *
 *  ***************************************************************************/

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

/*!
 *  @brief  Defines user input directions
 */
typedef enum {
    Nav_Up = 0,
    Nav_Down,
    Nav_Right,
    Nav_Left
} Nav;

/*!
 *  @brief  State machine states.
 */
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

/*!
 *  @brief      Singleton for a Finity Gauntlet state.
 */
typedef struct _FG_State {
    /*!
     *  The currently selected device type.
     */
    State_List device_type;

    /*!
     *   The currently selected command.
     */
    struct Command * selected_command;

    /*!
     *  The currently selected device.
     */
    struct Device * selected_device;
} FG_State;

extern FG_State outgoing_state; // Sent to the networking and graphics threads when the user sends a command.
extern FG_State FG_user_state; // The current Finity Gauntlet state.

extern void init_FG_state(); // Starts the state machine
extern bool update_FG_state(Nav i); // Updates the state machine using the current state and user input.

#ifdef __cplusplus
}
#endif

#endif /* TASKS_I2C_TASK_STATE_H_ */
