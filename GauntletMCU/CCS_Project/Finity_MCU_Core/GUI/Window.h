/*
 * canvas.h
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#ifndef GUI_WINDOW_H_
#define GUI_WINDOW_H_

#include "drivers/LCD/FG_graphics.h"
#include "util/cord.h"


class Window {
public:
    explicit Window(Graphics_Context * context, Cord2S loc);
    virtual void init()=0;

    virtual Graphics_Context * getContext();
    virtual void setContext(Graphics_Context * context);
    virtual Cord2S getloc();
    virtual void clr_region(Graphics_Rectangle region);
protected:
    virtual Cord2D transform_cord(int32_t x, int32_t y);
    virtual void drawString(int8_t *string, int32_t lLength, int32_t x, int32_t y, bool opaque);
    virtual void drawPixel(int32_t x, int32_t y, int32_t color);

private:
    Graphics_Context * context;
    Cord2S loc;
};


#endif /* GUI_WINDOW_H_ */
