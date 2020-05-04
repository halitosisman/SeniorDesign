/*
 * canvas.h
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

/*!****************************************************************************
 *  @file       Window.h
 *  @brief      Generic Finity Gauntlet GUI Window.
 *
 *  This is usually intended to be inherited.
 *  ***************************************************************************/

#ifndef GUI_WINDOW_H_
#define GUI_WINDOW_H_

#include "drivers/LCD/FG_graphics.h"
#include "util/cord.h"


class Window {
public:
    /*!
        \brief          Window

        Exists to satisfy the default constructor requirement. Does not do anything.

    */
    Window();

    /*!
        \brief          Window

        Initializes all the relevant StateTracker fields, yaddya ya.

        \param          Graphics_Context   -   The grlib Graphics context for this Window's draw calls.

        \param          Cord2S   -   Window location

        \param          Cord2S   -   Window size
    */
    explicit Window(Graphics_Context * context, Cord2S loc, Cord2S size);

    /*!
        \brief          init

        Empty currently.

     */
    virtual void init();

    /*!
        \brief          getContext

        Getter for this Window's Graphics_Context field.

        \return          Graphics_Context *   -   A pointer to this class'
    */
    virtual Graphics_Context * getContext();

    /*!
        \brief          setContext

        Setter for this Window's Graphics_Context field.

        \param          Graphics_Context *   -   Pointer to a Graphics_Context.
    */
    virtual void setContext(Graphics_Context * context);

    /*!
        \brief          getloc

        Getter for this Window's loc field.

        \return          Cord2S   -   This Window's location
    */
    virtual Cord2S getloc();

    /*!
        \brief          clr_region

        Clears the region specified by region.

        \param          Graphics_Rectangle   -   The region to be cleared given with Window coordinates.
    */
    virtual void clr_region(Graphics_Rectangle region);

    /*!
        \brief          setContext

        Setter for this Window's size field.

        \param          Cord2S   -   The size to set this window to.
    */
    virtual void setSize(Cord2S s);

    /*!
        \brief          getloc

        Getter for this Window's size field.

        \return          Cord2S   -   This Window's size.
    */
    virtual Cord2S getSize();

    /*!
        \brief          clear

        Clears this window.
    */
    virtual void clear();

    /*!
        \brief          get_string_height

        Getter for this Window's font height field.

        \return          short   -   The height of the font used for this Window.
    */
    virtual short get_string_height();

    /*!
        \brief          get_local_center

        Get the center local cords for this Window.

        \return          Cord2S   -   The center local cords for this Window.
    */
    virtual Cord2S get_local_center();
protected:
    /*!
        \brief          transform_cord

        Converts the input cords from Window local cords to global coordinates.

        \param          int32_t   -   x coordinate

        \param          int32_t   -   y coordinate

        \return         Cord2D    -   The input cords converted into global coordinates.
    */
    virtual Cord2D transform_cord(int32_t x, int32_t y);

    /*!
        \brief          drawString

        Draws a string at the given location.

        \param          int8_t *   -   Pointer to the input character buffer.

        \param          int32_t   -   Number of characters to print.

        \param          int32_t   -   Starting x coordinate of string

        \param          int32_t   -   Startin y coordinate of string.

        \param          bool   -   Whether to draw a background behind the string.
    */
    virtual void drawString(int8_t *string, int32_t lLength, int32_t x, int32_t y, bool opaque);

    /*!
        \brief          drawStringCentered

        Same as drawString, except that it centers the string on the input coordinates.

        \param          int8_t *   -   Pointer to the input character buffer.

        \param          int32_t   -   Number of characters to print.

        \param          int32_t   -   Starting x coordinate of string

        \param          int32_t   -   Startin y coordinate of string.

        \param          bool   -   Whether to draw a background behind the string.
    */
    virtual void drawStringCentered(int8_t *string, int32_t lLength, int32_t x, int32_t y, bool opaque);

    /*!
        \brief          drawPixel

        Draws a pixel at the specified location on the Window.

        \param          int32_t   -   Pixel x coordinate.

        \param          int32_t   -   Pixel y coordinate.

        \param          int32_t   -   Pixel color.
    */
    virtual void drawPixel(int32_t x, int32_t y, int32_t color);

private:
    Graphics_Context * context; // Graphics context for the grlib calls.
    Cord2S loc; // Window top left location.
    Cord2S size; // Window size.
};


#endif /* GUI_WINDOW_H_ */
