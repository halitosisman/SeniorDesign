/*
 * FG_GUI.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */


#include "FG_GUI.h"

void FG_GUI_init()
{
    FG_graphics_init();
}

FG_GUI::FG_GUI(Cord2S loc, Cord2S dim) : Window(&g_sContext, loc, dim)
{
    Cord2S s_size =
    {
     .x = static_cast<short>(Graphics_getDisplayWidth(this->getContext())),
     .y = static_cast<short>(Graphics_getDisplayHeight(this->getContext()))
    };

    Cord2S locs[Element_Cnt];

    locs[State_Tracker].x = 0;
    locs[State_Tracker].y = 0;


    Cord2S sizes[Element_Cnt];

    Logger logger = Logger();
    State_Tracker state_tracker = State_Tracker();
    DeviceDisplay device_display = DeviceDisplay();
}

FG_GUI::~FG_GUI()
{
}

void FG_GUI::update_state(int8_t* s1, uint8_t c1, int8_t* s2, uint8_t c2, int8_t* s3, uint8_t c3)
{
    this->state_tracker.update(s1, c1, s2, c2, s3, c3);
}
