/*
 * FG_GUI.h
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#ifndef GUI_FG_GUI_H_
#define GUI_FG_GUI_H_


#include <GUI/Logger.h>
#include <GUI/StateTracker.h>
#include <GUI/DeviceDisplay.h>

#ifdef __cplusplus
extern "C" {
#endif


class FG_GUI : public Window {
public:
    explicit FG_GUI(Cord2S loc, Cord2S dim);
    virtual ~FG_GUI();
    virtual void update_state(int8_t * s1, uint8_t c1, int8_t * s2, uint8_t c2, int8_t * s3, uint8_t c3);
    virtual void update_device_status(FG_State* state);
    virtual void update_device_info(FG_State* state);
    virtual void init();
private:
    Logger logger;
    StateTracker state_tracker;
    DeviceDisplay device_display;
};

#ifdef __cplusplus
}
#endif



#endif /* GUI_FG_GUI_H_ */
