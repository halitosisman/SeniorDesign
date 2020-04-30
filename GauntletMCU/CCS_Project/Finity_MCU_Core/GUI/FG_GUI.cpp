/*
 * FG_GUI.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */


#include "FG_GUI.h"



FG_GUI::FG_GUI(Cord2S loc, Cord2S dim) : Window(&g_sContext, loc, dim)
{
    short display_width = static_cast<short>(Graphics_getDisplayWidth(this->getContext()));
    short display_height = static_cast<short>(Graphics_getDisplayHeight(this->getContext()));

    Cord2S s_size =
    {
     .x = display_width,
     .y = display_height
    };
    this->setSize(s_size);

    Cord2S state_tracker_size =
    {
     .x = display_width,
     .y = static_cast<short>(15 + 2 * Graphics_getStringHeight(this->getContext()))
    };

    Cord2S state_tracker_pos = {0, 0};

    Cord2S logger_size =
    {
     .x = display_width,
     .y = static_cast<short>(Graphics_getStringHeight(this->getContext()) + 11)
    };

    Cord2S logger_pos = {0, static_cast<short>(display_height - logger_size.y - 1)};

    Cord2S device_tracker_size =
    {
     .x = display_width,
     .y = static_cast<short>(logger_pos.y - state_tracker_size.y)
    };

    Cord2S device_tracker_pos =
    {
     .x = 0,
     .y = state_tracker_size.y
    };

    this->logger = Logger(logger_pos, logger_size, "## ");
    this->state_tracker = StateTracker(state_tracker_pos, state_tracker_size);
    this->device_display = DeviceDisplay(device_tracker_pos, device_tracker_size);
}

FG_GUI::~FG_GUI()
{
}

void FG_GUI::update_state(int8_t* s1, uint8_t c1, int8_t* s2, uint8_t c2, int8_t* s3, uint8_t c3)
{
    this->state_tracker.update(s1, c1, s2, c2, s3, c3);
}

void FG_GUI::update_device_status(FG_State* state)
{
    this->device_display.update_status(state);
}

void FG_GUI::update_device_info(FG_State* state)
{
    this->device_display.update_device_info(state);
}

void FG_GUI::init()
{
    this->logger.init();
    this->state_tracker.init();
    this->device_display.init();
}

void FG_GUI::clear_device_status()
{
    this->device_display.clear();
}
