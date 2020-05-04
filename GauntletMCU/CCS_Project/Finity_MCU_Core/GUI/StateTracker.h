/*
 * StateTracker.h
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

/*!****************************************************************************
 *  @file       StateTracker.h
 *  @brief      Window for tracking the Finity Gauntlet user state.
 *  ***************************************************************************/

#ifndef GUI_STATETRACKER_H_
#define GUI_STATETRACKER_H_

#include <cstdarg>
#include <GUI/Window.h>

class StateTracker : public Window
{
public:
    /*!
        \brief          StateTracker

        Exists to satisfy the default constructor requirement. Does not do anything.

    */
    StateTracker();

    /*!
        \brief          StateTracker

        Initializes all the relevant StateTracker fields, yaddya ya.

        \param          Cord2S   -   StateTracker location

        \param          Cord2S   -   Size of the StateTracker element
    */
    StateTracker(Cord2S log_loc, Cord2S default_size);

    /*!
        \brief          ~StateTracker

        Doesn't do anything currently.

     */
    virtual ~StateTracker();

    /*!
        \brief          update

        Updates the state info displayed on screen.

        \param          int8_t *   -   Pointer to character label for state 1.

        \param          uint8_t   -   Number of characters pointed to by s1.

        \param          int8_t *   -   Pointer to character label for state 2. This is the user's currently selected
                                       state.

        \param          uint8_t   -   Number of characters pointed to by s2.

        \param          int8_t *   -   Pointer to character label for state 3.

        \param          uint8_t   -   Number of characters pointed to by s3.
    */
    virtual void update(int8_t * s1, uint8_t c1, int8_t * s2, uint8_t c2, int8_t * s3, uint8_t c3);

    /*!
        \brief          init

        Doesn't do anything currently.

     */
    virtual void init();
private:
    Cord2D locs[3];  // The local coordinates of the three state fields.
    uint8_t s_count; // The number of states.
};

#endif /* GUI_STATETRACKER_H_ */
