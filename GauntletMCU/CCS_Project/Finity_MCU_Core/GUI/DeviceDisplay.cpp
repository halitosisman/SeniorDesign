
/*
 * StateTracker.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#include <GUI/DeviceDisplay.h>


static Cord2S log_loc = {0, 0};
static Cord2S default_size = {300, 50};

DeviceDisplay::DeviceDisplay() : Window(&g_sContext, log_loc, default_size)
{
    Cord2S s =
    {
     .x = static_cast<short>(Graphics_getWidthOfDisplay(this->getContext()->display)),
     .y = 100
    };
    this->setSize(s);

 /*   this->locs[Element_Type] = {5, 5};  // Device Type
    this->locs[Element_ID] = {5, 30};  // Device ID
    this->locs[Element_Name] = {5, 55};  // Device Name
    this->locs[Element_Special_Type] = {100, 5};  // Device Special Type(dim, setable, etc)
    this->locs[Element_Special_Status] = {110, 30};  // Device Special status (dim, setable, etc)
    this->locs[Element_Special_Type2] = {100, 60}; // Device Special Type 2(dim, setable, etc)
    this->locs[Element_Special_Status2] = {110, 80}; // Device Special status 2(dim, setable, etc)
    this->locs[Element_Status_Static] = {170, 5};   // Device status static
    this->locs[Element_Status_Current] = {180, 5};   // Device status current*/
}

DeviceDisplay::~DeviceDisplay()
{
    // TODO Auto-generated destructor stub
}

void DeviceDisplay::init() {
}

void DeviceDisplay::update_status(FG_State* FG_curr_state)
{
    Cord2S loc = this->getloc();
    
    std::string curr_status = std::to_string(FG_curr_state->selected_device->status);
    int32_t length = curr_status.length();

    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawString((int8_t *)curr_status.c_str(), length - 1, this->locs[8].x, this->locs[8].y, true);
}

void DeviceDisplay::update_device_info(FG_State* FG_curr_state)
{
    this->clear();

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
