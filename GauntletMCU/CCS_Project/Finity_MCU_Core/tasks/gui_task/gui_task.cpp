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



QueueHandle_t sl_event_box;

int32_t fatfs_getFatTime(void) {
    return 0;
}


GUI_Letter gui_get_update(QueueHandle_t * mailroom);

void gui_task(void * par) {
    //GUI_Letter prev_state;
    GUI_Letter state;
    Logger logger = Logger();
    State_Tracker state_tracker = State_Tracker();

    FG_GUI_init();
    logger.init();
    state_tracker.init();
    while (1) {
        prev_state = state;
        state = gui_get_update(static_cast<FGthread_arg_t *>(par)->mailroom);
        if (state.cmd_issued) {
            // ignore for now
        }
        else {
            struct Command * cmd[3];
            strcpy(cmd[0]->name, " ");
            cmd[0]->command_len = sizeof(" ");
            strcpy(cmd[2]->name, " ");
            cmd[2]->command_len = sizeof(" ");

            if (state.state.selected_command != NULL) {
                if (state.state.selected_command->next != NULL) {
                    cmd[0] = state.state.selected_command->next;
                }
                if (state.state.selected_command->prev != NULL) {
                    cmd[2] = state.state.selected_command->prev;
                }
                cmd[1] = state.state.selected_command;
            }

            else if (state.state.selected_device != NULL) {
                if (state.state.selected_device->next != NULL) {
                    cmd[0] = (Command *) state.state.selected_device->next;
                }
                if (state.state.selected_device->prev != NULL) {
                    cmd[2] = (Command *) state.state.selected_device->prev;
                }
                cmd[1] = (Command *) state.state.selected_device;
            }
            else {
                switch (state.state.device_type) {
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
                    break;
                }
            }
            state_tracker.update(6, cmd[0]->name, cmd[0]->name_len, cmd[1]->name, cmd[1]->name_len, cmd[2]->name,
                                 cmd[2]->name_len);
        }
    }

}

GUI_Letter gui_get_update(QueueHandle_t* mailroom)
{
    GUI_Letter state;
    xQueueReceive(mailroom[GUI_THREAD_ID], &state, pdMS_TO_TICKS(portMAX_DELAY));
    return state;
}
