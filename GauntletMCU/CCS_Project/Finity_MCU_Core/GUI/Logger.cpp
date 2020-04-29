/*
 * logging.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#include <GUI/Logger.h>


static Cord2S log_loc = {0, 199};
static Cord2S default_size = {300, 20};

Logger::Logger() : Window(&g_sContext, log_loc, default_size)
{
    Cord2S s =
    {
     .x = Graphics_getWidthOfDisplay(this->getContext()->display),
     .y = Graphics_getHeightOfDisplay(this->getContext()->display) - 11 - Graphics_getStringHeight(this->getContext())
    };
    this->setSize(s);
    this->txt_start = {0, 0};
    this->prompt_start = {10, 5};
}

void Logger::print(int8_t * str, int size)
{
    Cord2S loc = this->getloc();
    Graphics_Rectangle clr_region =
    {
     .xMax = this->getSize().x,
     .xMin = this->txt_start.x,
     .yMax = this->getSize().y,
     .yMin = this->txt_start.y
    };
    this->clr_region(clr_region);
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawString(str, size, this->txt_start.x, this->txt_start.y, true);
}

void Logger::init()
{
    int8_t prompt_str[] = "## ";
    this->txt_start.x = this->prompt_start.x + Graphics_getStringWidth(this->getContext(),
                                                                       static_cast<int8_t *>(prompt_str),
                                                                       sizeof(prompt_str) - 1);
    this->txt_start.y = this->prompt_start.y;
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_DARK_SEA_GREEN);
    this->drawString(prompt_str, sizeof(prompt_str) - 1, this->prompt_start.x, this->prompt_start.y, true);
}

void Logger::print(int n)
{
}
