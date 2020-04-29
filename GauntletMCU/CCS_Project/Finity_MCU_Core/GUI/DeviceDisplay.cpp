
/*
 * StateTracker.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#include <GUI/DeviceDisplay.h>


static Cord2S log_loc = {0, 0};
static Cord2S default_size = {300, 50};

Device_Display::Device_Display() : Window(&g_sContext, log_loc, default_size)
{
    Cord2S s =
    {
     .x = static_cast<short>(Graphics_getWidthOfDisplay(this->getContext()->display)),
     .y = 100
    };
    this->setSize(s);

    this->locs[0] = {100, 10};  // Device Type
    this->locs[1] = {170, 10};  // Device ID
    this->locs[2] = {240, 10};  // Device Name
    for (int i = 0; i < 3; i++) { //access all the arguments assigned to valist
        this->s_bbox[i].xMax = this->locs[i].x + 60;
        this->s_bbox[i].xMin = this->locs[i].x;
        this->s_bbox[i].yMax = this->locs[i].y + 30;
        this->s_bbox[i].yMin = this->locs[i].y;
    }

    this->locs[3] = {100, 50};  // Device Special Type(dim, setable, etc)
    this->locs[4] = {110, 80};  // Device Special status (dim, setable, etc)
    this->locs[5] = {100, 120}; // Device Special Type 2(dim, setable, etc)
    this->locs[6] = {110, 150}; // Device Special status 2(dim, setable, etc)
    for (int i = 3; i < 7; i++) { //access all the arguments assigned to valist
        this->s_bbox[i].xMax = this->locs[i].x + 60;
        this->s_bbox[i].xMin = this->locs[i].x;
        this->s_bbox[i].yMax = this->locs[i].y + 30;
        this->s_bbox[i].yMin = this->locs[i].y;
    }

    this->locs[7] = {170, 50};   // Device status static
    this->s_bbox[7].xMax = this->locs[7].x + 60;
    this->s_bbox[7].xMin = this->locs[7].x;
    this->s_bbox[7].yMax = this->locs[7].y + 30;
    this->s_bbox[7].yMin = this->locs[7].y;

    this->locs[8] = {180, 90};   // Device status current
    this->s_bbox[8].xMax = this->locs[8].x + 80;
    this->s_bbox[8].xMin = this->locs[8].x;
    this->s_bbox[8].yMax = this->locs[8].y + 80;
    this->s_bbox[8].yMin = this->locs[8].y;
}

Device_Display::~Device_Display()
{
    // TODO Auto-generated destructor stub
}

void Device_Display::init() {
}

void Device_Display::update_status(FG_State* FG_curr_state)
{
    Cord2S loc = this->getloc();
    this->clr_region(this->s_bbox[8]);
    
    std::string curr_status = std::to_string(FG_curr_state->selected_device->status);
    int32_t length = curr_status.length();

    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawString((int8_t *)curr_status.c_str(), length - 1, this->locs[8].x, this->locs[8].y, true);
}

void Device_Display::update_device_info(FG_State* FG_curr_state)
{
    for(uint8_t i = 0; i < 9; i++) {
        this->clr_region(this->s_bbox[i]);
    }

    if (FG_curr_state->selected_device != nullptr) {
        switch(FG_curr_state->selected_device->ID[0])
        {
            case 'L':
                this->drawString((int8_t *)("Light:"), 6, this->locs[0].x, this->locs[0].y, true);
                this->drawString((int8_t *)("Dimmable:"), 10, this->locs[3].x, this->locs[3].y, true);
                if (FG_curr_state->selected_device->ID[1] == 'D') {
                    this->drawString((int8_t *)("True"), 4, this->locs[4].x, this->locs[3].y, true);
                }
                else {
                    this->drawString((int8_t *)("False"), 5, this->locs[4].x, this->locs[3].y, true);
                }
                break;
            case 'M':
                this->drawString((int8_t *)("Motor:"), 6, this->locs[0].x, this->locs[0].y, true);
                this->drawString((int8_t *)("Reverse:"), 8, this->locs[3].x, this->locs[3].y, true);
                if (FG_curr_state->selected_device->ID[1] == 'R') {
                    this->drawString((int8_t *)("True"), 4, this->locs[4].x, this->locs[3].y, true);
                }
                else {
                    this->drawString((int8_t *)("False"), 5, this->locs[4].x, this->locs[3].y, true);
                }
                this->drawString((int8_t *)("Setable:"), 8, this->locs[5].x, this->locs[3].y, true);
                if (FG_curr_state->selected_device->ID[2] == 'S') {
                    this->drawString((int8_t *)("True"), 4, this->locs[6].x, this->locs[3].y, true);
                }
                else {
                    this->drawString((int8_t *)("False"), 5, this->locs[6].x, this->locs[3].y, true);
                }
                break;
            case 'A':
                this->drawString((int8_t *)("Accel:"), 6, this->locs[0].x, this->locs[0].y, true);
                this->drawString((int8_t *)("Setable:"), 8, this->locs[3].x, this->locs[3].y, true);
                if (FG_curr_state->selected_device->ID[1] == 'S') {
                    this->drawString((int8_t *)("True"), 4, this->locs[4].x, this->locs[3].y, true);
                }
                else {
                    this->drawString((int8_t *)("False"), 5, this->locs[4].x, this->locs[3].y, true);
                }
                break;
            case 'T':
                this->drawString((int8_t *)("Tempr:"), 6, this->locs[0].x, this->locs[0].y, true);
                this->drawString((int8_t *)("Setable:"), 8, this->locs[3].x, this->locs[3].y, true);
                if (FG_curr_state->selected_device->ID[1] == 'S') {
                    this->drawString((int8_t *)("True"), 4, this->locs[4].x, this->locs[3].y, true);
                }
                else {
                    this->drawString((int8_t *)("False"), 5, this->locs[4].x, this->locs[3].y, true);
                }
                break;
        }
        }
        
        // Info
        this->drawString((int8_t *) FG_curr_state->selected_device->ID, 8, this->locs[1].x, this->locs[1].y, true);
        this->drawString((int8_t *) FG_curr_state->selected_device->name, FG_curr_state->selected_device->name_len, this->locs[2].x, this->locs[2].y, true);
        // status
        this->drawString((int8_t *)("Curr Status:"), 12, this->locs[7].x, this->locs[7].y, true);
        update_status(FG_curr_state);
    }
