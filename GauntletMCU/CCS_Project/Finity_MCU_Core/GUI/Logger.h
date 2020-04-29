/*
 * logging.h
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#ifndef GUI_LOGGER_H_
#define GUI_LOGGER_H_


#include <GUI/Window.h>

class Logger : public Window {
public:
    explicit Logger();
    virtual void print(int8_t * str, int size);
    virtual void print(int n);
    virtual void init();
private:
    Cord2S txt_start;
    Cord2S prompt_start;
};


#endif /* GUI_LOGGER_H_ */
