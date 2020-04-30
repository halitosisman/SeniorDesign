/*
 * StateTracker.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#include <GUI/StateTracker.h>


StateTracker::StateTracker(Cord2S log_loc, Cord2S default_size) : Window(&g_sContext, log_loc, default_size)
{
    this->locs[0] = {5, 5};
    this->locs[1] = {this->getSize().x / 2, this->get_string_height() * 2 + 5};
    this->locs[2] = {5, 5};
    this->s_count = 3;
}

StateTracker::~StateTracker()
{
    // TODO Auto-generated destructor stub
}

void StateTracker::init() {
}

void StateTracker::update(int8_t * s1, uint8_t c1, int8_t * s2, uint8_t c2, int8_t * s3, uint8_t c3)
{
    this->clear();
    Graphics_setBackgroundColor(this->getContext(), GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_CRIMSON);
    this->drawString(s1, c1, this->locs[0].x, this->locs[0].y, false);

    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_WHITE);
    this->drawStringCentered(s2, c2, this->locs[1].x, this->locs[1].y, false);

    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_CRIMSON);
    this->locs[2].x = this->getSize().x - 10 - static_cast<short>(Graphics_getStringWidth(this->getContext(), s3, c3));
    this->drawString(s3, c3, this->locs[2].x, this->locs[2].y, false);
}

StateTracker::StateTracker()
{
}
