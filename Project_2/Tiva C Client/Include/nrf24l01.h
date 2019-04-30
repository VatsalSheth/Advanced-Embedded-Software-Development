/**
 * File: nrf24l01.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by nrf24l01.c
 * Date: 4/29/2019
 */

#ifndef INCLUDE_NRF24L01_H_
#define INCLUDE_NRF24L01_H_

#include "basic.h"
#include "spi_driver.h"

#define nrf_gpio_base   GPIO_PORTL_BASE
#define nrf_csn     GPIO_PIN_1
#define nrf_irq     GPIO_PIN_2
#define nrf_ce      GPIO_PIN_3

//Command List
#define R_REGISTER(x) (x & 0x1f)
#define W_REGISTER(x) ((x & 0x1f) | 0x20)
#define R_RX_PAYLOAD  (0x61)
#define W_TX_PAYLOAD  (0xa0)
#define FLUSH_TX   (0xe1)
#define FLUSH_RX   (0xe2)
#define REUSE_TX_PL (0xe3)
#define R_RX_PL_WID (0x60)
#define W_ACK_PAYLOAD(x) ((x & 0x07) | 0xa8)
#define W_TX_PAYLOAD_NOACK  (0xb0)
#define NOP  (0xff)

//Register Map
#define CONFIG (0x00)
#define EN_AA (0x01)
#define EN_RXADDR (0x02)
#define SETUP_AW (0x03)
#define SETUP_RETR (0x04)
#define RF_CH (0x05)
#define RF_SETUP (0x06)
#define STATUS (0x07)
#define OBSERVE_TX (0x08)
#define RPD (0x09)
#define RX_ADDR_P(x) (0x0a + x)
#define TX_ADDR (0x10)
#define RX_PW_P(x) (0x11 + x)
#define FIFO_STATUS (0x17)
#define DYNPD (0x1c)
#define FEATURE (0x1d)

void clear_CSN();
void set_CSN();
void set_CE();
void clear_CE();
void ConfigureNRF(void);
extern void NRF_ISR(void);
uint8_t NRF_write(uint32_t command, uint8_t *data, uint8_t count);
uint8_t NRF_read(uint32_t command, uint8_t *data, uint8_t count);
void set_rxmode(void);
void set_txmode(void);

#endif /* INCLUDE_NRF24L01_H_ */
