
/*
 * StateTracker.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#include <GUI/DeviceDisplay.h>


DeviceDisplay::DeviceDisplay(Cord2S log_loc, Cord2S default_size) : Window(&g_sContext, log_loc, default_size)
{
    uint8_t cols = 2;
    uint8_t rows = (Element_Count + 1) / cols;
    short h = this->getSize().y / rows;
    short w = this->getSize().x / cols;
    Cord2S s = {w, h};

    Cord2S local_center =
    {
     .x = s.x / 2,
     .y = s.y / 2
    };
    Cord2S global_center =
    {
     .x = 0,
     .y = 0
    };
    Cord2S p;
    for (uint8_t c = 0; c < cols; c++) {
        for (uint8_t r = 0; r < rows; r++) {
            uint8_t j = c * rows + r;
            if (j >= Element_Count) {
                break;
            }
            p.x = c * s.x;
            p.y = r * s.y;
            this->elements[j] = Window(this->getContext(), p, s);
            global_center.x = local_center.x + this->elements[j].getloc().x;
            global_center.y = local_center.y + this->elements[j].getloc().y;
            this->centers[j] = global_center;
            this->buf_len[j] = 0;
        }
    }
}

DeviceDisplay::~DeviceDisplay()
{
    // TODO Auto-generated destructor stub
}

void DeviceDisplay::init() {
    for (uint8_t i = 0; i < Element_Count; i++) {
        this->elements[i].init();
    }
}

void DeviceDisplay::update_status(FG_State* FG_curr_state)
{
    Cord2S loc = this->getloc();
    
    std::string curr_status = std::to_string(FG_curr_state->selected_device->status);
    int32_t length = curr_status.length();

    this->buf_len[Element_Status_Current] = this->build_entry(static_cast<char *>("Curr Status "),
                                                              strlen(static_cast<char *>("Curr Status ")),
                                                              (char *) curr_status.c_str(), curr_status.length(),
                                                              Element_Status_Current);
    this->refresh_status();
}

void DeviceDisplay::update_device_info(FG_State* FG_curr_state)
{
    this->clear();

    if (FG_curr_state->selected_device != nullptr) {
        switch(FG_curr_state->selected_device->ID[0])
        {
            case 'L':
                this->buf_len[Element_Type] = this->write_element(static_cast<char *>("Light"), Element_Type);
                if (FG_curr_state->selected_device->ID[1] == 'D') {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Dimmable "),
                                                                            strlen(static_cast<char *>("Dimmable ")),
                                                                            static_cast<char *>("True"),
                                                                            strlen(static_cast<char *>("True")),
                                                                            Element_Special_Type);
                }
                else {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Dimmable "),
                                                                            strlen(static_cast<char *>("Dimmable ")),
                                                                            static_cast<char *>("False"),
                                                                            strlen(static_cast<char *>("False")),
                                                                            Element_Special_Type);
                }
                this->write_element(static_cast<char *>("  "), Element_Special_Type2);
                break;
            case 'M':
                this->buf_len[Element_Type] = this->write_element(static_cast<char *>("Motor"), Element_Type);
                if (FG_curr_state->selected_device->ID[1] == 'R') {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Reverse "),
                                                                            strlen(static_cast<char *>("Reverse ")),
                                                                            static_cast<char *>("True"),
                                                                            strlen(static_cast<char *>("True")),
                                                                            Element_Special_Type);
                }
                else {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Reverse "),
                                                                            strlen(static_cast<char *>("Reverse ")),
                                                                            static_cast<char *>("False"),
                                                                            strlen(static_cast<char *>("False")),
                                                                            Element_Special_Type);
                }
                if (FG_curr_state->selected_device->ID[2] == 'S') {
                    this->buf_len[Element_Special_Type2] = this->build_entry(static_cast<char *>("Settable "),
                                                                             strlen(static_cast<char *>("Settable ")),
                                                                             static_cast<char *>("True"),
                                                                             strlen(static_cast<char *>("True")),
                                                                             Element_Special_Type2);
                }
                else {
                    this->buf_len[Element_Special_Type2] = this->build_entry(static_cast<char *>("Settable "),
                                                                             strlen(static_cast<char *>("Settable ")),
                                                                             static_cast<char *>("False"),
                                                                             strlen(static_cast<char *>("False")),
                                                                             Element_Special_Type2);
                }
                break;
            case 'A':
                this->buf_len[Element_Type] = this->write_element(static_cast<char *>("Accel"), Element_Type);
                if (FG_curr_state->selected_device->ID[1] == 'S') {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Setable "),
                                                                            strlen(static_cast<char *>("Setable ")),
                                                                            static_cast<char *>("True"),
                                                                            strlen(static_cast<char *>("True")),
                                                                            Element_Special_Type);
                }
                else {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Setable "),
                                                                            strlen(static_cast<char *>("Setable ")),
                                                                            static_cast<char *>("False"),
                                                                            strlen(static_cast<char *>("False")),
                                                                            Element_Special_Type);
                }
                this->write_element(static_cast<char *>("  "), Element_Special_Type2);
                break;
            case 'T':
                this->buf_len[Element_Type] = this->write_element(static_cast<char *>("Tempr"), Element_Type);
                if (FG_curr_state->selected_device->ID[1] == 'S') {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Setable "),
                                                                            strlen(static_cast<char *>("Setable ")),
                                                                            static_cast<char *>("True"),
                                                                            strlen(static_cast<char *>("True")),
                                                                            Element_Special_Type);
                }
                else {
                    this->buf_len[Element_Special_Type] = this->build_entry(static_cast<char *>("Setable "),
                                                                            strlen(static_cast<char *>("Setable ")),
                                                                            static_cast<char *>("False"),
                                                                            strlen(static_cast<char *>("False")),
                                                                            Element_Special_Type);
                }
                this->write_element(static_cast<char *>("  "), Element_Special_Type2);
                break;
        }
        }
        
        // Info
        this->buf_len[Element_ID] = this->write_elementn(FG_curr_state->selected_device->ID, 8, Element_ID);
        this->buf_len[Element_Name] = this->write_elementn(FG_curr_state->selected_device->name, 8, Element_Name);
        // status
        update_status(FG_curr_state);
        this->refresh();
}

uint8_t DeviceDisplay::build_entry(char* label, uint8_t ll, char* data, uint8_t ld, Device_Elements i)
{
    if (ll < ELEMENT_MAX_CHAR_CNT) {
        strcpy(this->buf[i], label);
        if (ll + ld < ELEMENT_MAX_CHAR_CNT) {
            strcpy(&(this->buf[i][ll]), data);
        }
        else {
            strncpy(&(this->buf[i][ll]), data, ELEMENT_MAX_CHAR_CNT - ll);
        }
    }
    return ll + ld;
}

uint8_t DeviceDisplay::write_element(char* label, Device_Elements i)
{
    strcpy(this->buf[i], label);
    return strlen(label);
}

uint8_t DeviceDisplay::write_elementn(char* label, uint8_t n, Device_Elements i)
{
    strncpy(this->buf[i], label, n);
    return n;
}

DeviceDisplay::DeviceDisplay()
{
}

void DeviceDisplay::refresh()
{
    Cord2S c;
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    for (uint8_t i = 0; i < Element_Status_Current; i++) {
        if (this->buf_len[i] != 0) {
            this->drawStringCentered((int8_t *) this->buf[i], this->buf_len[i], this->centers[i].x, this->centers[i].y,
                                     false);
        }
    }
}

void DeviceDisplay::refresh_status()
{
    Cord2S c = this->elements[Element_Status_Current].get_local_center();
    Cord2S pos = this->elements[Element_Status_Current].getloc();
    c.x += pos.x;
    c.y += pos.y;
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawStringCentered((int8_t *) this->buf[Element_Status_Current], this->buf_len[Element_Status_Current],
                             c.x, c.y, false);

}
