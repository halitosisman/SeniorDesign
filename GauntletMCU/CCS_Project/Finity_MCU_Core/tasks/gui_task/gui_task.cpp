/*
 * net_task.c
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 *
 *
 *      Much of the initialization code here was copied from the Simplelink CC32xx SDK tcpecho example
 */


#include <tasks/gui_task/gui_task.h>



static mqd_t gui_mailroom;

int32_t fatfs_getFatTime(void) {
    return 0;
}


static bool gui_get_update()
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);

    char msg = 0;
    mq_receive(gui_mailroom, &msg, sizeof(msg), 0);
    if (msg == 1) {
        return true;
    }
    else {
        return false;
    }
}

struct Command empty =
{
 .name = " ",
 .name_len = sizeof(" ")
};
void gui_task(void * par) {
    gui_mailroom = ((FGthread_arg_t *) par)->mailroom[GUI_THREAD_ID];
    Logger logger = Logger();
    State_Tracker state_tracker = State_Tracker();
    Device_Display device_display = Device_Display();

    FG_GUI_init();
    logger.init();
    state_tracker.init();
    device_display.init();
    while (1) {
        struct Command default_command[3] =
        {
         empty,
         empty,
         empty
        };
        gui_get_update();
        struct Command * cmd[3] =
        {
         &(default_command[0]),
         &(default_command[1]),
         &(default_command[2])
        };
        pthread_mutex_lock(&list_sync);
        // User is not in the command selection screen
        if (FG_user_state.selected_command != NULL) {
            if (FG_user_state.selected_command->next != NULL) {
                cmd[0] = FG_user_state.selected_command->next;
            }
            if (FG_user_state.selected_command->prev != NULL) {
                cmd[2] = FG_user_state.selected_command->prev;
            }
            cmd[1] = FG_user_state.selected_command;
        }
        // User is not in the device selection screen
        else if (FG_user_state.selected_device != NULL) {
            if (FG_user_state.selected_device->next != NULL) {
                cmd[0] = (Command *) FG_user_state.selected_device->next;
            }
            if (FG_user_state.selected_device->prev != NULL) {
                cmd[2] = (Command *) FG_user_state.selected_device->prev;
            }
            cmd[1] = (Command *) FG_user_state.selected_device;
        }
        // Therefore the user must be in the device type selection screen!
        else {
            switch (FG_user_state.device_type) {
            case Device_Light:
                strcpy(cmd[1]->name, "Light");
                cmd[1]->name_len = sizeof("Light");
                strcpy(cmd[2]->name, "Motor");
                cmd[2]->name_len = sizeof("Motor");
                break;
            case Device_Motor:
                strcpy(cmd[0]->name, "Light");
                cmd[0]->name_len = sizeof("Light");
                strcpy(cmd[1]->name, "Motor");
                cmd[1]->name_len = sizeof("Motor");
                strcpy(cmd[2]->name, "Accel");
                cmd[2]->name_len = sizeof("Accel");
                break;
            case Device_Accel:
                strcpy(cmd[0]->name, "Motor");
                cmd[0]->name_len = sizeof("Motor");
                strcpy(cmd[1]->name, "Accel");
                cmd[1]->name_len = sizeof("Accel");
                strcpy(cmd[2]->name, "Temp");
                cmd[2]->name_len = sizeof("Temp");
                break;
            case Device_Temp:
                strcpy(cmd[0]->name, "Accel");
                cmd[0]->name_len = sizeof("Accel");
                strcpy(cmd[1]->name, "Temp");
                cmd[1]->name_len = sizeof("Temp");
                strcpy(cmd[2]->name, "System");
                cmd[2]->name_len = sizeof("System");
                break;
            case Device_System:
                cmd[0]->name_len = sizeof("Temp");
                strcpy(cmd[0]->name, "Temp");
                cmd[1]->name_len = sizeof("System");
                strcpy(cmd[1]->name, "System");
                break;
            }
        }
        state_tracker.update((int8_t *)(cmd[0]->name), cmd[0]->name_len,
                             (int8_t *)(cmd[1]->name), cmd[1]->name_len,
                             (int8_t *)(cmd[2]->name), cmd[2]->name_len);
        if(FG_user_state.selected_command != NULL) {
            device_display.update_status(&(FG_user_state));
        }
        else if (FG_user_state.selected_device != NULL) {
            device_display.update_device_info(&(FG_user_state));
        }
        pthread_mutex_unlock(&list_sync);
    }

}
