/*
 * basic.h
 *
 *  Created on: Apr 17, 2019
 *      Author: vkshe
 */

#ifndef INCLUDE_BASIC_H_
#define INCLUDE_BASIC_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"

//*****************************************************************************
//
// Global variable to hold the system clock speed.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

#endif /* INCLUDE_BASIC_H_ */
