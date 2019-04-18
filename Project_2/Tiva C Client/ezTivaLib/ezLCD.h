#ifndef __EZLCD_LIB__H
#define __EZLCD_LIB__H

#if defined(TM4C123GH6PM)
    #include "TM4C123GH6PM.H"
    typedef GPIOA_Type      GPIO_Port;
    typedef GPIOA_Type*     PGPIO_Port;

#elif defined(TM4C1294NCPDT)
    #include "TM4C1294NCPDT.h"
    typedef GPIOA_AHB_Type  GPIO_Port;
    typedef GPIOA_AHB_Type* PGPIO_Port;
#else
    #error "The Device NOT specified"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <string.h>     // memset
#include "ezDefines.h"
#include "ezTimer.h"

//typedef GPIOA_AHB_Type   GPIO_Port;
//typedef GPIOA_AHB_Type* PGPIO_Port;


typedef enum {
    PIN_3_0,
    PIN_7_4
} EZLCD_DATAPORT_PIN;

typedef struct EZOBJ_LCD{
    void*               PrivateData;
} EZOBJ_LCD;
typedef EZOBJ_LCD * PEZOBJ_LCD;

PEZOBJ_LCD  ezLCD_Create(void);

void    ezLCD_Connect_DataPort(PEZOBJ_LCD lcd, PGPIO_Port port, EZLCD_DATAPORT_PIN mode);
void    ezLCD_Connect_ENPin(PEZOBJ_LCD lcd, PGPIO_Port port, uint8_t enpin);
void    ezLCD_Connect_RSPin(PEZOBJ_LCD lcd, PGPIO_Port port, uint8_t rspin);
void    ezLCD_Connect_RWPin(PEZOBJ_LCD lcd, PGPIO_Port port, uint8_t rwpin);

bool    ezLCD_Start(PEZOBJ_LCD lcd);
void    ezLCD_Enable(PEZOBJ_LCD lcd);
void    ezLCD_DisplayOff(PEZOBJ_LCD lcd);
void    ezLCD_DisplayOn(PEZOBJ_LCD lcd);

void    ezLCD_ClearDisplay(PEZOBJ_LCD lcd);
void    ezLCD_Position(PEZOBJ_LCD lcd, uint8_t row, uint8_t column);
void    ezLCD_LoadVerticalBargraphFonts(PEZOBJ_LCD lcd);
void    ezLCD_LoadHorizontalBargraphFonts(PEZOBJ_LCD lcd);

void    ezLCD_LoadCustomFonts(PEZOBJ_LCD lcd, uint8_t const customData[]);

void    ezLCD_DrawHorizontalBG(PEZOBJ_LCD lcd, uint8_t row, uint8_t column, uint8_t maxCharacters, uint8_t value);
void    ezLCD_DrawVerticalBG(PEZOBJ_LCD lcd, uint8_t row, uint8_t column, uint8_t maxCharacters, uint8_t value);

void    ezLCD_PrintString(PEZOBJ_LCD lcd, char const str[]);
void    ezLCD_PrintInt8(PEZOBJ_LCD lcd, uint8_t value);
void    ezLCD_PrintInt16(PEZOBJ_LCD lcd, uint16_t value);
void    ezLCD_PrintInt32(PEZOBJ_LCD lcd, uint32_t value);
void    ezLCD_PrintNumber(PEZOBJ_LCD lcd, uint16_t value);
void    ezLCD_Print32Number(PEZOBJ_LCD lcd, uint32_t value);

#define ezLCD_PrintDecUint16(lcd,x)     ezLCD_PrintNumber(lcd,x)
#define ezLCD_PrintHexUint8(lcd,x)      ezLCD_PrintInt8(lcd,x)
#define ezLCD_PrintHexUint16(lcd,x)     ezLCD_PrintInt16(lcd,x)

#endif
