#ifndef __EZTIMER_LIB_H
#define __EZTIMER_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "ezDefines.h"



#if defined(TM4C123GH6PM)
    #include "TM4C123GH6PM.h"
#elif defined(TM4C1294NCPDT)
    #include "TM4C1294NCPDT.h"
#endif

#if defined(TM4C123GH6PM)
    void Setup_123G_80MHz(void);
    void Setup_123G_50MHz(void);
    void Setup_123G_40MHz(void);
    void Setup_123G_25MHz(void);
    void Setup_123G_20MHz(void);
    void Setup_123G_16MHz(void);
    void Setup_123G_8MHz(void);
#endif

#if defined(TM4C1294NCPDT)
    void Setup_1294_120MHz(void);
    void Setup_1294_96MHz(void);
    void Setup_1294_80MHz(void);
    void Setup_1294_60MHz(void);
    void Setup_1294_48MHz(void);
    void Setup_1294_40MHz(void);
    void Setup_1294_32MHz(void);
    void Setup_1294_30MHz(void);
    void Setup_1294_24MHz(void);
    void Setup_1294_20MHz(void);
    void Setup_1294_16MHz(void);
    void Setup_1294_12MHz(void);
    void Setup_1294_6MHz(void);
#endif


void timer_waitMillis(uint32_t millis);
void timer_waitMicros(uint16_t micros);



#endif
