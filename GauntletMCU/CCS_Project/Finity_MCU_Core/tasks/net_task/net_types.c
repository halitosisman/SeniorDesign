/*
 * net_types.c
 *
 *  Created on: Apr 25, 2020
 *      Author: halitosisman
 */


#include "net_types.h"

pthread_mutex_t list_sync;

struct Device_List device_list =
{
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0
};

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

