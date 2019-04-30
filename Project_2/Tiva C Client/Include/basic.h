/**
 * File: basic.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables used by all files.
 * Date: 4/29/2019
 */


#ifndef INCLUDE_BASIC_H_
#define INCLUDE_BASIC_H_

#include "FreeRTOSConfig.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/timer.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

struct __attribute__((__packed__)) tx_packet
{
    float latitude;
    float longitude;
    uint8_t status;
    uint32_t t_s;
};

//*****************************************************************************
//
// Global variable to hold the system clock speed.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

#endif /* INCLUDE_BASIC_H_ */
