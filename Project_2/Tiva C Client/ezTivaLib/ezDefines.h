
#ifndef __EZDEFINES_H
#define __EZDEFINES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/*
#if defined(TM4C123GH6PM)
    #include "TM4C123GH6PM.h"
#elif defined(TM4C1294NCPDT)
    #include "TM4C1294NCPDT.h"
#endif
*/

#ifndef PIN0
#define PIN0    0x01
#define PIN1    0x02
#define PIN2    0x04
#define PIN3    0x08
#define PIN4    0x10
#define PIN5    0x20
#define PIN6    0x40
#define PIN7    0x80
#endif

#ifndef BIT0
#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80
#endif


#endif
