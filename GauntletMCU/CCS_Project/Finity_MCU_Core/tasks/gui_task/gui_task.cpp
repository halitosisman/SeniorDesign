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
    Logger logger = Logger();
    State_Tracker state_tracker = State_Tracker();

    int8_t msg[] = "Hello, World!";
    int8_t s1[] = "State 1";
    int8_t s2[] = "State 2";
    int8_t s3[] = "State 3";

    FG_GUI_init();
    logger.init();
    state_tracker.init();
    while (1) {
        logger.print(msg, sizeof(msg));
        state_tracker.update(6, s1, sizeof(s1) - 1, s2, sizeof(s2) - 1, s3, sizeof(s3) - 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
