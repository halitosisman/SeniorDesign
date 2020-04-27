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
    this->s_size.x = 80;
    this->s_size.y = 30;
    this->s_count = 3;
    for (int i = 0; i < 3; i++) { //access all the arguments assigned to valist
        this->s_bbox[i].xMax = this->locs[i].x + this->s_size.x;
        this->s_bbox[i].xMin = this->locs[i].x;
        this->s_bbox[i].yMax = this->locs[i].y + this->s_size.y;
        this->s_bbox[i].yMin = this->locs[i].y;
    }
}

State_Tracker::~State_Tracker()
{
    // TODO Auto-generated destructor stub
}

void State_Tracker::init() {
}

void State_Tracker::update(int8_t * s1, uint8_t c1, int8_t * s2, uint8_t c2, int8_t * s3, uint8_t c3)
{
    Cord2S loc = this->getloc();
    for(uint8_t i = 0; i < 3; i++) {
        this->clr_region(this->s_bbox[i]);
    }
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawString(s1, c1, this->locs[0].x, this->locs[0].y, true);
    this->drawString(s2, c2, this->locs[1].x, this->locs[1].y, true);
    this->drawString(s3, c3, this->locs[2].x, this->locs[2].y, true);
}
