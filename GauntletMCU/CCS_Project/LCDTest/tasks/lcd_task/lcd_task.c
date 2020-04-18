/*
 * net_task.c
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 *
 *
 *      Much of the initialization code here was copied from the Simplelink CC32xx SDK tcpecho example
 */


#include <tasks/lcd_task/lcd_task.h>



QueueHandle_t sl_event_box;


int32_t fatfs_getFatTime(void) {
    return 0;
}


void gui_task(void * par) {
    FG_graphics_init();
    FG_graphics_test();
    while (1) {
    }

}
