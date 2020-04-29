/*
 * StateTracker.h
 *
 *  Created on: Apr 19, 2020
 *      Author: halitosisman
 */

#ifndef GUI_STATETRACKER_H_
#define GUI_STATETRACKER_H_

#include <cstdarg>
#include <GUI/Window.h>

class State_Tracker : public Window
{
public:
    State_Tracker();
    virtual ~State_Tracker();
    virtual void update(int8_t * s1, uint8_t c1, int8_t * s2, uint8_t c2, int8_t * s3, uint8_t c3);
    virtual void init();
private:
    Cord2D locs[3];
    uint8_t s_count;
};

#endif /* GUI_STATETRACKER_H_ */
