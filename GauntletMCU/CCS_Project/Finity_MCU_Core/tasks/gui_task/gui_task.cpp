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


void gui_task(void * par) {
    GUI_Letter state;
    Logger logger = Logger();
    State_Tracker state_tracker = State_Tracker();

    FG_GUI_init();
    logger.init();
    state_tracker.init();
    while (1) {
        state = gui_get_update(static_cast<FGthread_arg_t *>(par)->mailroom);
        logger.print(state.logger, state.l_items);
        state_tracker.update(6, state.state_tracker[GUI_THREAD_STATE_0_ID], state.st_items[GUI_THREAD_STATE_0_ID] - 1,
                             state.state_tracker[GUI_THREAD_STATE_1_ID], state.st_items[GUI_THREAD_STATE_1_ID] - 1,
                             state.state_tracker[GUI_THREAD_STATE_2_ID], state.st_items[GUI_THREAD_STATE_2_ID] - 1);
    }

}

GUI_Letter gui_get_update(QueueHandle_t* mailroom)
{
    GUI_Letter state;
    xQueueReceive(mailroom[GUI_THREAD_ID], &state, pdMS_TO_TICKS(portMAX_DELAY));
    return state;
}
