/*
 * DeviceElement.h
 *
 *  Created on: Apr 29, 2020
 *      Author: halitosisman
 */

#ifndef GUI_DEVICEELEMENT_H_
#define GUI_DEVICEELEMENT_H_

#include <GUI/Window.h>

class DeviceElement: public Window
{
public:
    DeviceElement(Cord2S loc, Cord2S dim);
    virtual ~DeviceElement();
};

#endif /* GUI_DEVICEELEMENT_H_ */
