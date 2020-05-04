/*
 * logging.h
 *
 *  Created on: Apr 18, 2020
 *      Author: halitosisman
 */

/*!****************************************************************************
 *  @file       Logger.h
 *  @brief      Finity Gauntlet top level GUI class.
 *
 *  Can be used to display event messages to the user.
 *
 *  ***************************************************************************/

#ifndef GUI_LOGGER_H_
#define GUI_LOGGER_H_


#include <cstring>
#include <GUI/Window.h>

class Logger : public Window {
public:
    /*!
        \brief          Logger

        Exists to satisfy the default constructor requirement. Does not do anything.

    */
    Logger();

    /*!
        \brief          Logger

        Initializes all the relevant Logger fields, yaddya ya.

        \param          Cord2S   -   Logger location

        \param          Cord2S   -   Size of the logger element

        \param          char *   -   Pointer to a null terminated string containing the prompt text for the logger.
    */
    explicit Logger(Cord2S log_loc, Cord2S default_size, char * prompt);

    /*!
        \brief          print

        Prints text to the logger.

        \param          int8_t *   -   Pointer to a text array to print.

        \param          int   -   The number of items to print.
    */
    virtual void print(int8_t * str, int size);

    /*!
        \brief          init

        Draws the prompt.
    */
    virtual void init();
private:
    short txt_height; // Height of the font used for the Logger.
    Cord2S txt_start; // Starting coordinate of Logger messages.
    Cord2S prompt_start; // Starting coordinate of the Logger prompt.
    uint8_t prompt_cnt; // Number of items in the prompt string.
    int8_t prompt_str[20]; // Buffer for the prompt.
};


#endif /* GUI_LOGGER_H_ */
