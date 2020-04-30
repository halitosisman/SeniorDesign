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
#include <GUI/Logger.h>

#include "state.h"


#define ELEMENT_MAX_CHAR_CNT 25


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
    explicit DeviceDisplay(Cord2S log_loc, Cord2S default_size);
    virtual ~DeviceDisplay();
    virtual void update_status(FG_State*);
    virtual void update_device_info(FG_State*);
    virtual void init();
protected:
    virtual void build_entry(char* label, uint8_t ll, char* data, uint8_t ld, Device_Elements i);
    virtual void write_element(char * label, Device_Elements i);
    virtual void write_elementn(char * label, uint8_t n, Device_Elements i);
private:
    Window elements[Element_Count];
    char buf[Element_Count][ELEMENT_MAX_CHAR_CNT];
    Cord2S centers[Element_Count];
};

#endif /* GUI_DEVICEDISPLAY_H_ */
