/*
 * logging.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#include <GUI/Logger.h>


Logger::Logger(Cord2S log_loc, Cord2S default_size, char * prompt) : Window(&g_sContext, log_loc, default_size)
{
    int32_t prompt_width = 0;
    this->txt_height = 0;
    short txt_loc = 0;

    this->prompt_cnt = strlen(prompt);
    strcpy((char *)(this->prompt_str), prompt);

    prompt_width = Graphics_getStringWidth(this->getContext(), this->prompt_str, this->prompt_cnt);
    txt_height = this->get_string_height();
    txt_loc = static_cast<short>((this->getSize().y - txt_height) / 2);
    this->txt_start = {static_cast<short>(prompt_width), txt_loc};
    this->prompt_start = {0, txt_loc};
}

void Logger::print(int8_t * str, int size)
{
    Cord2S loc = this->getloc();
    Graphics_Rectangle clr_region =
    {
     .xMax = this->getSize().x,
     .xMin = this->txt_start.x,
     .yMax = static_cast<short>(this->txt_start.y + this->txt_height),
     .yMin = this->txt_start.y
    };
    this->clr_region(clr_region);
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawString(str, size, this->txt_start.x, this->txt_start.y, true);
}

void Logger::init()
{
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawString(this->prompt_str, this->prompt_cnt, this->prompt_start.x, this->prompt_start.y, false);
}

void Logger::print(int n)
{
}

Logger::Logger()
{
}
