/*
 * canvas.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#include <GUI/Window.h>

Window::Window(Graphics_Context * context, Cord2D loc)
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
    //Graphics_drawString(&(this->context), string, lLength, c.x, c.y, opaque);
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

Cord2D Window::getloc()
{
    return this->loc;
}
