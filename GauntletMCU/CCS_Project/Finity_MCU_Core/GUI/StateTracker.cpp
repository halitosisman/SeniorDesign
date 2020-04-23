/*
 * StateTracker.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#include <GUI/StateTracker.h>


static Cord2S log_loc = {0, 0};

State_Tracker::State_Tracker() : Window(&g_sContext, log_loc)
{
    this->locs[0] = {10, 10};
    this->locs[1] = {10, 50};
    this->locs[2] = {10, 90};
    this->s_size.x = 50;
    this->s_size.y = 30;
}

State_Tracker::~State_Tracker()
{
    // TODO Auto-generated destructor stub
}

void State_Tracker::init() {

}

void State_Tracker::update(int num, ...)
{
    Cord2S loc = this->getloc();
    Graphics_Rectangle clr_region;
    va_list valist;
    va_start(valist, num);
    int8_t * state;
    for (int i = 0; i < num; i += 2) { //access all the arguments assigned to valist
        clr_region.xMax = this->locs[i / 2].x + this->s_size.x;
        clr_region.xMin = this->locs[i / 2].x;
        clr_region.yMax = this->locs[i / 2].y + this->s_size.y;
        clr_region.yMin = this->locs[i / 2].y;
        this->clr_region(clr_region);
        state = va_arg(valist, int8_t*);
        Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
        this->drawString(state, va_arg(valist, int), this->locs[i / 2].x, this->locs[i / 2].y, true);
    }
    va_end(valist);
}
