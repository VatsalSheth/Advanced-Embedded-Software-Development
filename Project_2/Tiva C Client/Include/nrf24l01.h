/*
 * nrf24l01.h
 *
 *  Created on: Apr 18, 2019
 *      Author: vkshe
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

//Register Map
#define CONFIG (0x00)
#define EN_RXADDR (0x02)
#define RX_ADDR_P(x) (0x0a + x)
#define TX_ADDR (0x10)
#define RX_PW_P(x) (0x11 + x)

void clear_CSN();
void set_CSN();
void set_CE();
void clear_CE();
void ConfigureNRF(void);
void NRF_ISR(void);
uint8_t NRF_write(uint8_t command, uint8_t *data, uint8_t count);
uint8_t NRF_read(uint8_t command, uint8_t *data, uint8_t count);

#endif /* INCLUDE_NRF24L01_H_ */
