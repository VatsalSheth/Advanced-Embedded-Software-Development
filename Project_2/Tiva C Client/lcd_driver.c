/**
 * File: lcd_driver.c
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file has driver API for 16x2 character LCD
 * Date: 4/29/2019
 */


#include "Include/lcd_driver.h"

uint8_t row_addr[4] = {0x00, 0x40, 0x10, 0x50};

/**
 * @brief Write to instruction register of LCD
 * @param data Command to be written in instruction register
 */
void WR_CMD(uint8_t data)
{
    GPIOPinTypeGPIOOutput(data_gpio_base, 0xff);
    GPIOPinWrite(ctrl_gpio_base, RS|RW, 0x00);
    SysCtlDelay(6);      //Delay 50ns RS,RW setup time to EN
    GPIOPinWrite(ctrl_gpio_base, EN, EN);
    GPIOPinWrite(data_gpio_base, 0xff, data);
    SysCtlDelay(10);      //Delay 80ns Data setup time
    GPIOPinWrite(ctrl_gpio_base, EN, 0x00);
    SysCtlDelay(2);      //Delay 16ns Data, RS, RW hold time
    GPIOPinWrite(data_gpio_base, 0xff, 0x00);
    SysCtlDelay(59950);       //Delay 50 us
}

/**
 * @brief Read from instruction register of LCD
 * @return Instruction register value
 */
uint8_t RD_CMD(void)
{
    uint8_t data;

    GPIOPinTypeGPIOInput(data_gpio_base, 0xff);
    GPIOPinWrite(ctrl_gpio_base, RS|RW, RW);
    SysCtlDelay(6);      //Delay 50ns RS,RW setup time to EN
    GPIOPinWrite(ctrl_gpio_base, EN, EN);
    SysCtlDelay(20);      //Delay 160ns Data delay time
    data = (uint8_t)GPIOPinRead(data_gpio_base, 0xff);
    GPIOPinWrite(ctrl_gpio_base, EN, 0x00);
    SysCtlDelay(2);      //Delay 16ns Data, RS, RW hold time
    return data;
}

/**
 * @brief Write to data register of LCD
 * @param data Value to be written in data register
 */
void WR_DATA(uint8_t data)
{
    GPIOPinTypeGPIOOutput(data_gpio_base, 0xff);
    GPIOPinWrite(ctrl_gpio_base, RS|RW, RS);
    SysCtlDelay(6);      //Delay 50ns RS,RW setup time to EN
    GPIOPinWrite(ctrl_gpio_base, EN, EN);
    GPIOPinWrite(data_gpio_base, 0xff, data);
    SysCtlDelay(10);      //Delay 80ns Data setup time
    GPIOPinWrite(ctrl_gpio_base, EN, 0x00);
    SysCtlDelay(2);      //Delay 16ns Data, RS, RW hold time
    GPIOPinWrite(data_gpio_base, 0xff, 0x00);
    SysCtlDelay(59950);       //Delay 50 us
}

/**
 * @brief Read from data register of LCD
 * @return Data register value
 */
uint8_t RD_DATA(void)
{
    uint8_t data;

    GPIOPinTypeGPIOInput(data_gpio_base, 0xff);
    GPIOPinWrite(ctrl_gpio_base, RS|RW, RS|RW);
    SysCtlDelay(6);      //Delay 50ns RS,RW setup time to EN
    GPIOPinWrite(ctrl_gpio_base, EN, EN);
    SysCtlDelay(20);      //Delay 160ns Data delay time
    data = (uint8_t)GPIOPinRead(data_gpio_base, 0xff);
    GPIOPinWrite(ctrl_gpio_base, EN, 0x00);
    SysCtlDelay(2);      //Delay 16ns Data, RS, RW hold time
    return data;
}

/**
 * @brief Configure GPIO port M for data and port F for control signals of LCD. LCD initialization steps.
 */
void ConfigureLCD(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);  //Update in case of port change for
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(data_gpio_base, 0xff);
    GPIOPinTypeGPIOOutput(ctrl_gpio_base, (EN | RS | RW));

    SysCtlDelay(1798561);      //Delay 15 ms
    WR_CMD(0x38);   //Send function set 8bit data and 2 line interface
    SysCtlDelay(599520);       //Delay 5 ms
    WR_CMD(0x38);   //Send function set 8bit data and 2 line interface
    SysCtlDelay(599520);       //Delay 5 ms
    WR_CMD(0x38);   //Send function set 8bit data and 2 line interface
    lcdbusywait();
    WR_CMD(0x0C);   //Send Display on/off. D=1, C=0, B=0
    lcdbusywait();
    WR_CMD(0x06);   //Send Entry mode set, Increment cursor, NO display shift
    lcdclear();
}

/**
 * @brief Polls the LCD busy flag. Function does not return until the LCD controller is ready to accept another command.
 */
void lcdbusywait()
{
    uint8_t tmp;
    do
    {
        tmp = RD_CMD();
        tmp &= 0x80; //Check busy flag
    }
    while(tmp);
}

/**
 * @brief Sets the cursor to the specified LCD DDRAM address. Should call lcdbusywait().
 * @param addr DDRAM address to set cursor
 */
void lcdgotoaddr(uint8_t addr)
{
    lcdbusywait();
    WR_CMD(0x80 | (addr & 0x7f));
}

/**
 * @brief Sets the cursor to the LCD DDRAM address corresponding to the specified row and column. Location (0,0) is the top left corner of the LCD screen.
 * @param row Line number 0-3
 * @param column Column number 0-15
 */
void lcdgotoxy(uint8_t row, uint8_t column)
{
    uint8_t addr;
    addr = row_addr[row] + column;
    lcdgotoaddr(addr);
}

/**
 * @brief Writes the specified character to the current
 * @param cc Character to be written on LCD
 */
void lcdputch(char cc)
{
    lcdbusywait();
    WR_DATA(cc);
}

/**
 * @brief Writes the specified null-terminated string to the LCD starting at the current LCD cursor position. Automatically wraps
 * long strings to the next LCD line after the right edge of the display screen has been reached.
 * @param ss String to be displayed on LCD
 */
void lcdputstr(char *ss)
{
    uint8_t addr, i;
    addr = RD_CMD();
    addr &= 0x7f;

    while(*ss != '\0')
    {
        lcdputch(*(ss++));

        if(((addr) % 16)==15)
        {
            addr = addr & 0xf0;
            for(i=0; i<4; i++)
            {
                if(addr==row_addr[i])
                {
                    addr = row_addr[(i+1)%4];
                    break;
                }
            }
            lcdgotoaddr(addr);
        }
        else
        {
            addr+=1;
        }
    }
}

/**
 * @brief Displays integer number upto 3 decimal
 * @param a Input integer number to be displayed
 */
void lcdputint(int a)
{
    uint8_t i = 3, valid = 0;
    uint32_t div = 100;

    while(i != 0)
    {
        if(a/div || valid == 1 || i==1)
        {
            valid = 1;
            lcdputch(0x30 + a/div);
        }
        a = a % div;
        div = div/10;
        i--;
    }
}

/**
 * @brief Displays float number upto 4 decimal points
 * @param a Input float number to be displayed
 */
void lcdputfloat(float a)
{
    uint8_t i;

    if(a<0)
    {
        lcdputch('-');
        a = a*(-1);
        lcdputint((int)a);
    }
    else
    {
        lcdputint((int)a);
    }

    a = a - (int)a;
    lcdputch('.');
    for(i=0; i<4; i++)
    {
        a = a*10;
        lcdputch(0x30 + (int)a);
        a = a - (int)a;
    }
}

/**
 * @brief Function to create a 5x8 pixel custom character with character code ccode (0<=ccode<=7) using the row values given in
 * the 8-byte array row_vals[].
 * @param ccode Character code 0-7
 * @param row_vals Array of row values to generate character. Bit value 1 represents dark pixel.
 */
void lcdcreatechar(uint8_t ccode, uint8_t row_vals[8])
{
    uint8_t i;
    lcdbusywait();
    i = (0x40 |((ccode&0x07)<<3));
    WR_CMD(i);
    for(i=0; i<8; i++)
    {
        lcdbusywait();
        WR_DATA(row_vals[i]);
    }
}

/**
 * @brief Clears the LCD using the HD44780 Clear display instruction.
 */
void lcdclear()
{
    lcdbusywait();
    WR_CMD(0x01); //Clear Display
}
