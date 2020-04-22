/*
 * StateTracker.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#include <GUI/StateTracker.h>


static Cord2D log_loc = {0, 0};

State_Tracker::State_Tracker() : Window(&g_sContext, log_loc)
{
    this->locs[0] = {10, 10};
    this->locs[1] = {10, 50};
    this->locs[2] = {10, 90};
}

State_Tracker::~State_Tracker()
{
    // TODO Auto-generated destructor stub
}

void State_Tracker::init() {

}

void State_Tracker::update(int num, ...)
{
    va_list valist;
    va_start(valist, num);
    for (int i = 0; i < num; i += 2) { //access all the arguments assigned to valist
        int8_t * state = va_arg(valist, int8_t*);
        Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
        this->drawString(state, va_arg(valist, int), this->locs[i / 2].x, this->locs[i / 2].y, true);
    }
    va_end(valist);
}
