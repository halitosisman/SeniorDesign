/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       C3220SF_LAUNCHXL.h
 *
 *  @brief      CC3220 Board Specific APIs
 *
 *  The CC3220SF_LAUNCHXL header file should be included in an application as
 *  follows:
 *  @code
 *  #include <CC3220SF_LAUNCHXL.h>
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC3220SF_LAUNCHXL_H
#define __CC3220SF_LAUNCHXL_H


#include "stdint.h"

/* Common interface includes                                                 */
#include "tasks/net_task/network_if.h"
#include "util/uart_term.h"


#ifdef __cplusplus
extern "C" {
#endif

UART_Handle uart_debug;

typedef enum CC3220SF_LAUNCHXL_SPIName {
    CC3220SF_LAUNCHXL_SPI0 = 0,

    CC3220SF_LAUNCHXL_SPICOUNT
} CC3220SF_LAUNCHXL_SPIName;

typedef enum {
    GPIO_RDX = 0,
    GPIO_WRX,
    GPIO_DCX,
    GPIO_D7,
    GPIO_D6,
    GPIO_D5,
    GPIO_D4,
    GPIO_D3,
    GPIO_D2,
    GPIO_D1,
    GPIO_D0,
    GPIO_I2C_Int,
    GPIO_Pin_Count
} GPIO_Settings;

void i2c_int_callback(uint_least8_t index);

extern void CC3220SF_LAUNCHXL_initGeneral(void);

#ifdef __cplusplus
}
#endif

#endif /* __CC3220SF_LAUNCHXL_H */
