/*
 * StateTracker.h
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

/*!****************************************************************************
 *  @file       DeviceDisplay.h
 *  @brief      Window for holding information on the currently selected
 *              device.
 *
 * Hardcoded to display the fields shown in Device_Elements.
 *
 *  ***************************************************************************/

#ifndef GUI_DEVICEDISPLAY_H_
#define GUI_DEVICEDISPLAY_H_

#include <string>
#include <cstdarg>
#include <cstdint>
#include <GUI/Window.h>
#include <GUI/Logger.h>

#include "state.h"


#define ELEMENT_MAX_CHAR_CNT 25


/*!
 *  @brief  Defines different fields for holding selected device information.
 */
typedef enum {
    Element_Type = 0,
    Element_ID,
    Element_Name,
    Element_Special_Type,
    Element_Special_Type2,
    Element_Status_Current,
    Element_Count
} Device_Elements;


/*!
 *  @brief  Defines the values for the settings registers.
 *
 *  @sa AD7993_init()
 */
class DeviceDisplay : public Window
{
public:
    /*!
        \brief          DeviceDisplay

           Exists to satisfy the default constructor requirement. Does not do anything.

     */
    DeviceDisplay();

    /*!
        \brief          DeviceDisplay

        More efficient routine for writing command - parameter sequences

        \param          Cord2S   -   DeviceDisplay location on screen.

        \param          Cord2S   -   Starting size of DeviceDisplay.
     */
    explicit DeviceDisplay(Cord2S log_loc, Cord2S default_size);

    /*!
        \brief          ~DeviceDisplay

        Doesn't do anything currently.

     */
    virtual ~DeviceDisplay();

    /*!
        \brief          update_status

        Updates the Element_Status_Current element.

        \param          FG_State*   -   The current Finity Gauntlet state.

        \return         void

     */
    virtual void update_status(FG_State*);

    /*!
        \brief          update_device_info

        Updates all elements encapsulated under DeviceDisplay.

        \param          FG_State*   -   The bytes to be written in order of low index to high

        \return         void

        \note

     */
    virtual void update_device_info(FG_State*);

    /*!
        \brief          init

        Initialization function.

        \return         void

        \note

     */
    virtual void init();
protected:

    /*!
        \brief          build_entry

        Updates the Device_Elements entry of buf, with the concatenation of the two input strings and returns the length
        of the composite string.

        \param          char*   -   A pointer to the first string

        \param          uint8_t   -   The length of the first string.

        \param          char*   -   A pointer to the second string

        \param          uint8_t   -   The length of the second string.

        \param          Device_Elements   -   The buf index to update.

        \return         uint8_t  - The length of the composite string.

        \note

     */
    virtual uint8_t build_entry(char* label, uint8_t ll, char* data, uint8_t ld, Device_Elements i);

    /*!
        \brief          write_element

        Write the null terminated string label to the buf indexed by i.

        \param          char *   -   The null terminated string.

        \param          Device_Elements   -   The buf to update.

        \return         uint8_t     - the number of chars written to buf.

        \note

     */
    virtual uint8_t write_element(char * label, Device_Elements i);

    /*!
            \brief          write_elementn

            Write n characters from label to the buf indexed by i.

            \param          char *   -   The null terminated string.

            \param          uint8_t   -   The number of characters to write.

            \param          Device_Elements   -   The buf to update.

            \return         uint8_t     - the number of chars written to buf.

            \note

     */
    virtual uint8_t write_elementn(char * label, uint8_t n, Device_Elements i);

    /*!
                \brief          refresh

                Write the contents of all internal device buffers except status to the screen.

     */
    virtual void refresh();

    /*!
            \brief          refresh

            Only update the contents of the status device with the class buffer.

     */
    virtual void refresh_status();
private:
    Window elements[Element_Count]; // The Windows for displaying individual device data
    char buf[Element_Count][ELEMENT_MAX_CHAR_CNT]; // The text contained within the individual windows
    uint8_t buf_len[Element_Count]; // The number of meaningful items stored in each window's buf.
    Cord2S centers[Element_Count]; // The center coordinates of each window with the coordinate reference of the window.
};

#endif /* GUI_DEVICEDISPLAY_H_ */
