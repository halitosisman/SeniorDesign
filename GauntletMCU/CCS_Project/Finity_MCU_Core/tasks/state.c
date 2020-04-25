/*
 * i2c_event.c
 *
 *  Created on: Apr 24, 2020
 *      Author: halitosisman
 */


#include "state.h"


static bool device_null_callback(uint8_t event);
static bool device_light_callback(uint8_t event);
static bool device_motor_callback(uint8_t event);
static bool device_accel_callback(uint8_t event);
static bool device_temp_callback(uint8_t event);
static bool command_callback(uint8_t event);

FG_State FG_user_state =
{
 .device_type = Device_Motor,
 .selected_command = NULL,
 .selected_device = NULL
 };

static bool (*state_callbacks[State_Count])(uint8_t event) =
{
 device_null_callback,
 device_light_callback,
 device_motor_callback,
 device_accel_callback,
 device_temp_callback,
 command_callback
};

static bool (*present_callback)(uint8_t event);

bool update_FG_state(uint8_t i)
{
    return present_callback(i);
}

void init_FG_state()
{
    present_callback = state_callbacks[Device_Null];
}


// Note that the concept of direction here is relative to the state enum as viewed in the next editor, where upwards
// corresponds to lower indices
static bool device_null_callback(Nav event) {
    switch (event) {
    case Nav_Up:
        if (FG_user_state.device_type == Device_Null + 1) {
        }
        else {
            FG_user_state.device_type -= 1;
        }
        return false;
    case Nav_Down:
        if (FG_user_state.device_type == Device_Count - 1) {
        }
        else {
            FG_user_state.device_type += 1;
        }
        return false;
    case Nav_Right:
        present_callback = state_callbacks[FG_user_state.device_type];
        switch (FG_user_state.device_type) {
        case Device_Light:
            FG_user_state.selected_device = (struct Device *)(device_list.Light);
            return false;
        case Device_Motor:
            FG_user_state.selected_device = (struct Device *)(device_list.Motor);
            return false;
        case Device_Accel:
            FG_user_state.selected_device = (struct Device *)(device_list.Accel);
            return false;
        case Device_Temp:
            FG_user_state.selected_device = (struct Device *)(device_list.Temp);
            return false;
        case Device_System:
            FG_user_state.selected_command = command_list.S;
            return false;
        default:
            ERR_PRINT(LOGIC_ERROR);
        }
        return false;
    case Nav_Left:
        return false;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    return false;
}

static bool device_light_callback(uint8_t event) {
    struct Light * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Light *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        return false;
    case Nav_Down:
        if ((struct Light *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        return false;
    case Nav_Right:
        temp = (struct Light *)(FG_user_state.selected_device);
        if (temp->dimmable) {
            FG_user_state.selected_command = command_list.LD;
        }
        else {
            FG_user_state.selected_command = command_list.L;
        }
        present_callback = state_callbacks[Command];
        return false;
    case Nav_Left:
        present_callback = state_callbacks[Device_Null];
        return false;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    return false;
}

static bool device_motor_callback(uint8_t event) {
    struct Motor * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Motor *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        return false;
    case Nav_Down:
        if ((struct Motor *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        return false;
    case Nav_Right:
        temp = (struct Motor *)(FG_user_state.selected_device);
        if (temp->setable && temp->reversable) {
            FG_user_state.selected_command = command_list.MRS;
        }
        else if (temp->setable){
            FG_user_state.selected_command = command_list.MS;
        }
        else if (temp->reversable) {
            FG_user_state.selected_command = command_list.MR;
        }
        else {
            FG_user_state.selected_command = command_list.M;
        }
        present_callback = state_callbacks[Command];
        return false;
    case Nav_Left:
        present_callback = state_callbacks[Device_Null];
        return false;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    return false;
}

static bool device_accel_callback(uint8_t event) {
    struct Accel * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Accel *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        return false;
    case Nav_Down:
        if ((struct Accel *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        return false;
    case Nav_Right:
        temp = (struct Accel *)(FG_user_state.selected_device);
        if (temp->setable){
            FG_user_state.selected_command = command_list.AF;
        }
        else {
            FG_user_state.selected_command = command_list.A;
        }
        present_callback = state_callbacks[Command];
        return false;
    case Nav_Left:
        present_callback = state_callbacks[Device_Null];
        return false;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    return false;
}

static bool device_temp_callback(uint8_t event) {
    struct Temp * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Temp *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        return false;
    case Nav_Down:
        if ((struct Temp *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        return false;
    case Nav_Right:
        temp = (struct Temp *)(FG_user_state.selected_device);
        if (temp->setable){
            FG_user_state.selected_command = command_list.TF;
        }
        else {
            FG_user_state.selected_command = command_list.T;
        }
        present_callback = state_callbacks[Command];
        return false;
    case Nav_Left:
        present_callback = state_callbacks[Device_Null];
        return false;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    return false;
}
static bool command_callback(uint8_t event) {
    switch (event) {
    case Nav_Up:
        if (FG_user_state.selected_command->next != NULL) {
            FG_user_state.selected_command = FG_user_state.selected_command->next;
        }
        return false;
    case Nav_Down:
        if (FG_user_state.selected_command->prev != NULL) {
            FG_user_state.selected_command = FG_user_state.selected_command->prev;
        }
        return false;
    case Nav_Right:
        // TODO update and reset
        return false;
    case Nav_Left:
        FG_user_state.selected_command = NULL;
        present_callback = state_callbacks[Command];
        return false;
    default:
        break;
    }
    return false;
}

