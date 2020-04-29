/*
 * StateTracker.h
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#ifndef GUI_DEVICEDISPLAY_H_
#define GUI_DEVICEDISPLAY_H_

#include <string>
#include <cstdarg>
#include <cstdint>
#include <GUI/Window.h>

#include "state.h"


typedef enum {
    Element_Type = 0,
    Element_ID,
    Element_Name,
    Element_Special_Type,
    Element_Special_Status,
    Element_Special_Type2,
    Element_Special_Status2,
    Element_Status_Static,
    Element_Status_Current,
    Element_Count
} Device_Elements;


class DeviceDisplay : public Window
{
public:
    DeviceDisplay();
    virtual ~DeviceDisplay();
    virtual void update_status(FG_State*);
    virtual void update_device_info(FG_State*);
    virtual void init();
private:
    Cord2D locs[Element_Count];
    Cord2D s_size;
    uint8_t s_count;
};

#endif /* GUI_DEVICEDISPLAY_H_ */
