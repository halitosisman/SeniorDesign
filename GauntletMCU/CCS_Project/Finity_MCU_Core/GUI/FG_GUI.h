/*
 * FG_GUI.h
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

/*!****************************************************************************
 *  @file       FG_GUI.h
 *  @brief      Finity Gauntlet top level GUI class.
 *
 *  ***************************************************************************/

#ifndef GUI_FG_GUI_H_
#define GUI_FG_GUI_H_


#include <GUI/Logger.h>
#include <GUI/StateTracker.h>
#include <GUI/DeviceDisplay.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief  FG_GUI is a container class for the three major Finity Gauntlet GUI elements.
 */
class FG_GUI : public Window {
public:
    /*!
       \brief          FG_GUI

        Calls the constructors for the Finity Gauntlet GUI elements.

        \param          Cord2S   -   Location of the entire GUI. Traditionally the origin.

        \param          Cord2S   -   Size of the entire GUI. Traditionally the size of the screen.
    */
    explicit FG_GUI(Cord2S loc, Cord2S dim);

    /*!
        \brief          ~DeviceDisplay

        Doesn't do anything currently.

    */
    virtual ~FG_GUI();

    /*!
        \brief          update_state

        Updates the state_tracker portion of the GUI.

        \param          int8_t *   -   Pointer to character label for state 1.

        \param          uint8_t   -   Number of characters pointed to by s1.

        \param          int8_t *   -   Pointer to character label for state 2. This is the user's currently selected
                                       state.

        \param          uint8_t   -   Number of characters pointed to by s2.

        \param          int8_t *   -   Pointer to character label for state 3.

        \param          uint8_t   -   Number of characters pointed to by s3.
    */
    virtual void update_state(int8_t * s1, uint8_t c1, int8_t * s2, uint8_t c2, int8_t * s3, uint8_t c3);

    /*!
        \brief          update_device_status

        Updates the device status window contained by DeviceDisplay.

        \param          FG_State*   -   Pointer to the current Finity Gauntlet State (pre command).
    */
    virtual void update_device_status(FG_State* state);

    /*!
        \brief          clear_device_status

        Clear the device status window.
    */
    virtual void clear_device_status();

    /*!
        \brief          update_device_info

        Updates the device info fields on the DeviceDisplay window.

        \param          FG_State*   -   Pointer to state that the device info should reflect.
    */
    virtual void update_device_info(FG_State* state);

    /*!
        \brief          init

        Calls the init functions of all FG_GUI windows.
    */
    virtual void init();
private:
    Logger logger; // not actually used. Was intended to display event messages to the user.
    StateTracker state_tracker; // Displays the currently selected state and any immediately selectable states.
    DeviceDisplay device_display; // Displays info about the currently selected device.
};

#ifdef __cplusplus
}
#endif



#endif /* GUI_FG_GUI_H_ */
