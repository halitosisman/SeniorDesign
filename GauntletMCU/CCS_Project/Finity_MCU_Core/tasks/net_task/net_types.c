/*
 * net_types.c
 *
 *  Created on: Apr 25, 2020
 *      Author: halitosisman
 */


#include "net_types.h"

pthread_mutex_t list_sync;


/*
 * Initalize the device_list global that stores the headers of
 * the corresponding connected devices linked lists
 *
 * When a specific device type is selected, these headers are
 * the first devices selected in the new system state
 */
struct Device_List device_list =
{
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0
};

/*
 * Initalize the command_list global that stores the headers of
 * the selected device type's publishable commands linked list
 *
 * When a specific device is selected, the corresponding command
 * linked list is selected and displayed
 */
struct Command_List command_list =
{
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
};