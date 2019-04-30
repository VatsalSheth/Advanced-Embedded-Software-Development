/***********************************************************************************
* @nrf24L01.h
* @This file contains dependent include files and variable declaration for nrf24L01.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#ifndef NRF24L01_H_
#define NRF24L01_H_

#include "spi_driver.h"

#define NRF_SPI_CE	(50)
#define NRF_SPI_IRQ	(51)

#define REGISTER_MASK	(0x0F)

//Command List
#define R_REGISTER(x)	 (x & 0x1f)
#define W_REGISTER(x)	 ((x & 0x1f) | 0x20)
#define R_RX_PAYLOAD 	 (0x61)
#define W_TX_PAYLOAD 	 (0xa0)
#define FLUSH_TX  	 (0xe1)
#define FLUSH_RX  	 (0xe2)
#define REUSE_TX_PL	 (0xe3)
#define R_RX_PL_WID	 (0x60)
#define W_ACK_PAYLOAD(x) ((x & 0x07) | 0xa8)
#define W_TX_PAYLOAD_NOACK  (0xb0)
#define NOP 		 (0xff)

//Register Map
#define CONFIG_REG	 (0x00)
#define EN_AA_REG	 (0x01)
#define EN_RXADDR_REG	 (0x02)
#define SETUP_AW_REG	 (0x03)
#define SETUP_RETR_REG	 (0x04)
#define RF_CH_REG  	 (0x05)
#define RF_SETUP_REG 	 (0x06)
#define STATUS_REG	 (0x07)
#define OBSERVE_TX_REG 	 (0x08)
#define RPD_REG		 (0x09)
#define RX_ADDR_P(x)	 (0x0a + x)
#define TX_ADDR_REG	 (0x10)
#define RX_PW_P(x)	 (0x11 + x)
#define FIFO_STATUS_REG	 (0x17)
#define DYNPD_REG	 (0x1c)
#define FEATURE_REG	 (0x1d)

//actions
#define RX_FIFO_IRQ	(0b01000000)
#define TX_FIFO_IRQ	(0b00100000)
#define MAX_RT_IRQ	(0b00010000)

//packet macros
#define ACCIDENT_RX 	(0b00000001)
#define TIME_EXC_RX 	(0b00000010)
#define N_IGNORE_RX 	(0b00000100)
#define CONTINUE_RX 	(0b00001000)
#define GPS_RX	 	(0b00010000)
#define ACCEL_RX 	(0b00100000)

//#define FORWARD 	(0x00)
//#define BACKWARD 	(0x01)
//#define LEFT		(0x02)
//#define RIGHT	 	(0x03)
#define DIRECTION_TX	(0b00001111)
#define MISLED_TX 	(0b00010000)
#define REACHED_TX 	(0b00100000)

struct __attribute__((__packed__)) tiva_2_bbg
{
	float lati, longi;
	uint8_t status;
	uint32_t seconds;
}rx_packet;

struct __attribute__((__packed__)) bbg_2_tiva
{
	float lati, longi;
	uint8_t status;
}tx_packet;

void clear_CSN();
void set_CSN();
void set_CE();
void clear_CE();
void ConfigureNRF(void);
uint8_t NRF_write(uint8_t command, uint8_t *data, uint8_t count);
uint8_t NRF_read(uint8_t command, uint8_t *data, uint8_t count);

#endif
