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

FG_State outgoing_state =
{
 .device_type = Device_Motor,
 .selected_command = NULL,
 .selected_device = NULL
 };

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
static bool device_null_callback(uint8_t event) {
    switch (event) {
    case Nav_Up:
        if (FG_user_state.device_type == Device_Null + 1) {
        }
        else {
            FG_user_state.device_type -= 1;
        }
        break;
    case Nav_Down:
        if (FG_user_state.device_type == Device_Count - 1) {
        }
        else {
            FG_user_state.device_type += 1;
        }
        break;
    case Nav_Right:
        present_callback = state_callbacks[FG_user_state.device_type];
        switch (FG_user_state.device_type) {
        case Device_Light:
            if (device_list.Light != NULL) {
                FG_user_state.selected_device = (struct Device *)(device_list.Light);
            }
            else {
                present_callback = state_callbacks[Device_Null];
            }
            break;
        case Device_Motor:
            if (device_list.Motor != NULL) {
                FG_user_state.selected_device = (struct Device *)(device_list.Motor);
            }
            else {
                present_callback = state_callbacks[Device_Null];
            }
            break;
        case Device_Accel:
            if (device_list.Accel != NULL) {
                FG_user_state.selected_device = (struct Device *)(device_list.Accel);
            }
            else {
                present_callback = state_callbacks[Device_Null];
            }
            break;
        case Device_Temp:
            if (device_list.Temp != NULL) {
                FG_user_state.selected_device = (struct Device *)(device_list.Temp);
            }
            else {
                present_callback = state_callbacks[Device_Null];
            }
            break;
        case Device_System:
            if (command_list.S != NULL) {
                FG_user_state.selected_command = command_list.S;
            }
            else {
                present_callback = state_callbacks[Device_Null];
            }
            break;
        default:
            ERR_PRINT(LOGIC_ERROR);
        }
        break;
    case Nav_Left:
        break;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    outgoing_state = FG_user_state;
    return false;
}

static bool device_light_callback(uint8_t event) {
    struct Light * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Light *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        break;
    case Nav_Down:
        if ((struct Light *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        break;
    case Nav_Right:
        temp = (struct Light *)(FG_user_state.selected_device);
        if (temp->dimmable) {
            FG_user_state.selected_command = command_list.LD;
        }
        else {
            FG_user_state.selected_command = command_list.L;
        }
        present_callback = state_callbacks[Device_Command];
        break;
    case Nav_Left:
        FG_user_state.selected_device = NULL;
        present_callback = state_callbacks[Device_Null];
        break;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    outgoing_state = FG_user_state;
    return false;
}

static bool device_motor_callback(uint8_t event) {
    struct Motor * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Motor *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        break;
    case Nav_Down:
        if ((struct Motor *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        break;
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
        present_callback = state_callbacks[Device_Command];
        break;
    case Nav_Left:
        FG_user_state.selected_device = NULL;
        present_callback = state_callbacks[Device_Null];
        break;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    outgoing_state = FG_user_state;
    return false;
}

static bool device_accel_callback(uint8_t event) {
    struct Accel * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Accel *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        break;
    case Nav_Down:
        if ((struct Accel *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        break;
    case Nav_Right:
        temp = (struct Accel *)(FG_user_state.selected_device);
        if (temp->setable){
            FG_user_state.selected_command = command_list.AF;
        }
        else {
            FG_user_state.selected_command = command_list.A;
        }
        present_callback = state_callbacks[Device_Command];
        break;
    case Nav_Left:
        FG_user_state.selected_device = NULL;
        present_callback = state_callbacks[Device_Null];
        break;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    outgoing_state = FG_user_state;
    return false;
}

static bool device_temp_callback(uint8_t event) {
    struct Temp * temp;
    switch (event) {
    case Nav_Up:
        if ((struct Temp *)(FG_user_state.selected_device)->next != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->next;
        }
        break;
    case Nav_Down:
        if ((struct Temp *)(FG_user_state.selected_device)->prev != NULL) {
            FG_user_state.selected_device = FG_user_state.selected_device->prev;
        }
        break;
    case Nav_Right:
        temp = (struct Temp *)(FG_user_state.selected_device);
        if (temp->setable){
            FG_user_state.selected_command = command_list.TF;
        }
        else {
            FG_user_state.selected_command = command_list.T;
        }
        present_callback = state_callbacks[Device_Command];
        break;
    case Nav_Left:
        FG_user_state.selected_device = NULL;
        present_callback = state_callbacks[Device_Null];
        break;
    default:
        ERR_PRINT(LOGIC_ERROR);
    }
    outgoing_state = FG_user_state;
    return false;
}
static bool command_callback(uint8_t event) {
    switch (event) {
    case Nav_Up:
        if (FG_user_state.selected_command->next != NULL) {
            FG_user_state.selected_command = FG_user_state.selected_command->next;
        }
        break;
    case Nav_Down:
        if (FG_user_state.selected_command->prev != NULL) {
            FG_user_state.selected_command = FG_user_state.selected_command->prev;
        }
        break;
    case Nav_Right:
        // TODO double check this with Daniel
        outgoing_state = FG_user_state;
        if (FG_user_state.device_type != Device_System &&
                FG_user_state.selected_command->name_len == KILL_DEVICE_LEN &&
                strncmp(FG_user_state.selected_command->command, KILL_DEVICE, KILL_DEVICE_LEN) == 0) {
            if (FG_user_state.selected_device->next != NULL) {
                FG_user_state.selected_device->next->prev = FG_user_state.selected_device->prev;
            }
            if (FG_user_state.selected_device->prev != NULL) {
                FG_user_state.selected_device->prev->next = FG_user_state.selected_device->next;
            }
        }
        FG_user_state.device_type = Device_Motor;
        FG_user_state.selected_command = NULL;
        FG_user_state.selected_device = NULL;
        present_callback = state_callbacks[Device_Null];
        return true; // no state update, but trigger action
    case Nav_Left:
        FG_user_state.selected_command = NULL;
        if (FG_user_state.device_type == Device_System) {
            FG_user_state.selected_device = NULL;
            present_callback = state_callbacks[Device_Null];
        }
        else {
            present_callback = state_callbacks[FG_user_state.device_type];
        }
        break;
    default:
        break;
    }
    return false;
}

