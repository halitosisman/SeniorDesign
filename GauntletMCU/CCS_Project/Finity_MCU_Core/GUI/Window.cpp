/*
 * canvas.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#include <GUI/Window.h>

Window::Window(Graphics_Context * context, Cord2S loc)
{
    this->context = context;
    this->loc = loc;
}

Cord2D Window::transform_cord(int32_t x, int32_t y)
{
    Cord2D ret = {x + this->loc.x, y + this->loc.y};
    return ret;
}

void Window::drawString(int8_t* string, int32_t lLength, int32_t x, int32_t y, bool opaque)
{
    Cord2D c = this->transform_cord(x, y);
    Graphics_drawString(this->context, string, lLength, c.x, c.y, opaque);
}

Graphics_Context * Window::getContext()
{
    return this->context;
}

void Window::setContext(Graphics_Context * context)
{
    this->context = context;
}

Cord2S Window::getloc()
{
    return this->loc;
}

void Window::clr_region(Graphics_Rectangle region)
{
    Graphics_Rectangle clr_region =
    {
     .xMax = this->loc.x + region.xMax + 1,
     .xMin = this->loc.x + region.xMin - 1,
     .yMax = this->loc.y + region.yMax + 1,
     .yMin = this->loc.y + region.yMin - 1
    };
    Graphics_setForegroundColor(this->getContext(), GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(this->getContext(), &clr_region);
}

void Window::drawPixel(int32_t x, int32_t y, int32_t color)
{
    Cord2D c = this->transform_cord(x, y);
    Graphics_setForegroundColor(this->getContext(), color);
    Graphics_drawPixel(this->context, c.x, c.y);
}
