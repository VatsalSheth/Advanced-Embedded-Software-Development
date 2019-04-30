#include "../Include/nrf24L01.h"

/**
 * @brief Sets CSN pin of NRF24l01+ module
 */
void set_CSN()
{
	gpio_set_value(GPIO_SPI_CS, HIGH);
}

/**
 * @brief Sets CE pin of NRF24l01+ module
 */
void set_CE()
{
	gpio_set_value(NRF_SPI_CE, HIGH);
}

/**
 * @brief Clears CE pin of NRF24l01+ module
 */
void clear_CE()
{
	gpio_set_value(NRF_SPI_CE, LOW);
}

/**
 * @brief Clears CSN pin of NRF24l01+ module
 */
void clear_CSN()
{
	gpio_set_value(GPIO_SPI_CS, HIGH);
	gpio_set_value(GPIO_SPI_CS, LOW);
}

/**
 * @brief Configures NRF24L01+ module
 */
void ConfigureNRF(void)
{
	uint8_t rc;
    	uint8_t val[5];

//	gpio_export(GPIO_SPI_CS);
	gpio_dir(GPIO_SPI_CS, "out");
	gpio_dir(NRF_SPI_CE, "out");
  	spi_dev_init();
    
//    GPIOIntRegister(nrf_gpio_base, NRF_ISR);
//    GPIOIntTypeSet(nrf_gpio_base, nrf_irq, GPIO_FALLING_EDGE);
//    GPIOIntEnable(nrf_gpio_base, GPIO_INT_PIN_2);       //Update in case of IRQ pin change

//    SPI_rxfifo_flush();

	rc = NRF_read(R_REGISTER(CONFIG_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(EN_AA_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(EN_RXADDR_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(SETUP_AW_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(SETUP_RETR_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(RF_CH_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(RF_SETUP_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(STATUS_REG), &val[0], 1);
	rc = NRF_read(R_REGISTER(OBSERVE_TX_REG), &val[0], 1);

	rc = NRF_read(R_REGISTER(RX_ADDR_P(1)), val, 5);

	if((val[0] == 0xC2)
	 &&(val[1] == 0xC2)
	 &&(val[2] == 0xC2)
	 &&(val[3] == 0xC2)
	 &&(val[4] == 0xC2))
		printf("NRF module success...!!! \n");
	else 
		printf("NRF module fail...!!! \n");
    	
//	val[0] = 0x64;
//  	rc = NRF_write(W_REGISTER(RF_CH_REG), &val[0], 1);    //Set channel frequency 2.4GHz + 100MHz
	val[0] = 0x01;
    	rc = NRF_write(W_REGISTER(EN_RXADDR_REG), &val[0], 1);    //Enable data pipe 0 Only
    	val[0] = 0x06;
    	val[1] = 0x06;
    	val[2] = 0x06;
    	val[3] = 0x00;
    	val[4] = 0x00;
    	rc = NRF_write(W_REGISTER(RX_ADDR_P(0)), val, 5);    //Data pipe 0 Rx addr is 0x06
    	val[0] = 0x09;
    	val[1] = 0x09;
    	val[2] = 0x09;
    	val[3] = 0x00;
    	val[4] = 0x00;
    	rc = NRF_write(W_REGISTER(TX_ADDR_REG), val, 5);    //Tx addr is 0x06
    	val[0] = sizeof(struct tiva_2_bbg);
    	rc = NRF_write(W_REGISTER(RX_PW_P(0)), &val[0], 1);    //Static payload in Data pipe 0 is size of struct tiva_2_bbg

    	val[0] = 0x0b;  //Rx
//    	val[0] = 0x0a;  //Tx
    	rc = NRF_write(W_REGISTER(CONFIG_REG), &val[0], 1);    //Power up mode & Tx/Rx mode
/*    	val[0]=1;
    	val[3]=2;
    	rc = NRF_write(W_TX_PAYLOAD, &val[0], 4);
    	val[0]=3;
  	val[3]=4;
    	rc = NRF_write(W_TX_PAYLOAD, &val[0], 4);
    	set_CE();
*/
	val[0] = NOP;
	rc - NRF_write(FLUSH_TX, &val[0], 1);
	rc - NRF_write(FLUSH_RX, &val[0], 1);
}

/**
 * @brief Write to NRF24l01+ module
 * @param command NRF24l01+ command to send
 * @param data Pointer of data to send
 * @param count Number of bytes of data to send from above pointer. count=0 for read commands
 * @return Status register value
 */
uint8_t NRF_write(uint8_t command, uint8_t *data, uint8_t count)
{
    	uint8_t i, rc, temp[15];
	temp[0] = command;

	if(count)
    	{
        	for(i=1; i<=count; i++)
        	{
            		temp[i] = data[i-1];
        	}
    	}

    	rc = spi_write(command, temp, count+1);
	return rc;
}

/**
 * @brief Read from NRF24l01+ module
 * @param command NRF24l01+ command to read
 * @param data Pointer of data to receive
 * @param count Number of bytes of data to receive from above pointer
 * @return Status register value
 */
uint8_t NRF_read(uint8_t command, uint8_t *data, uint8_t count)
{
    	uint8_t rc;

    	clear_CSN();
	rc = spi_read(command, data, count);
    	set_CSN();
    	return rc;
}

