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


class Device_Display : public Window
{
public:
    Device_Display();
    virtual ~Device_Display();
    virtual void update_status(FG_State*);
    virtual void update_device_info(FG_State*);
    virtual void init();
private:
    Cord2D locs[9];
    Cord2D s_size;
    uint8_t s_count;
    Graphics_Rectangle s_bbox[9];
};

#endif /* GUI_DEVICEDISPLAY_H_ */
