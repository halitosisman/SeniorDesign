/*
 * logging.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#include <GUI/Logger.h>

/*Graphics_Display log_display =
{
     .displayData = NULL, // TODO Move hardcoded values to config at some point
     .heigth = 240,
     .pFxns = g_sContext.display->pFxns,
     .size = sizeof(Graphics_Display),
     .width = 320
};*/


static Cord2D log_loc = {0, 199};

Logger::Logger() : Window(&g_sContext, log_loc)
{
    this->txt_start = {0, 0};
    this->prompt_start = {10, 20};
}

void Logger::print(int8_t * str, int size)
{
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
