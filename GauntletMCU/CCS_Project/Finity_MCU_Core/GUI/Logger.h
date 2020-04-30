/*
 * logging.h
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

#ifndef GUI_LOGGER_H_
#define GUI_LOGGER_H_


#include <cstring>
#include <GUI/Window.h>

class Logger : public Window {
public:
    Logger();
    explicit Logger(Cord2S log_loc, Cord2S default_size, char * prompt);
    virtual void print(int8_t * str, int size);
    virtual void print(int n);
    virtual void init();
private:
    short txt_height;
    Cord2S txt_start;
    Cord2S prompt_start;
    uint8_t prompt_cnt;
    int8_t prompt_str[20];
};


#endif /* GUI_LOGGER_H_ */
