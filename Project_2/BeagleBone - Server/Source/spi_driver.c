#include "../Include/spi_driver.h"

/**
 * @brief Initialized SPI device with a number of RD WR modes
 */
void spi_dev_init()
{
	spi_fd = open(device, O_RDWR);
	if(spi_fd < 0)
	{
		printf("Failed to open SPI bus");
		exit(1);
	}

	rc_check = ioctl(spi_fd, SPI_IOC_WR_MODE32, &mode);
	if(rc_check == -1)
	{
		printf("Cant set SPI mode");
		exit(1);
	}

	rc_check = ioctl(spi_fd, SPI_IOC_RD_MODE32, &mode);
	if(rc_check == -1)
	{
		printf("Cant get SPI mode");
		exit(1);
	}
	
	rc_check = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if(rc_check == -1)
	{
		printf("Cant set bits per word");
		exit(1);
	}

	rc_check = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if(rc_check == -1)
	{
		printf("Cant get bits per word");
		exit(1);
	}
	
	rc_check = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(rc_check == -1)
	{
		printf("Cant set max speed hz");
		exit(1);
	}

	rc_check = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if(rc_check == -1)
	{
		printf("Cant get max speed hz");
		exit(1);
	}
}

/**
 * @brief Write to SPI device 
 *
 * @param address to be written at of NRF chip
 * @param byte data to be written to module
 * @param count number of bytes to be written
 *
 * @return SUCCESS or FAILURE
 */
uint8_t spi_write(uint8_t address, uint8_t* byte, uint8_t count)
{
	gpio_set_value(GPIO_SPI_CS, HIGH);
	gpio_set_value(GPIO_SPI_CS, LOW);

	rc_check = write(spi_fd, byte, count);
	if(rc_check != count)
	{
		printf("Register address write failed");
		return FAILURE;
//		exit(1);
	}

	gpio_set_value(GPIO_SPI_CS, HIGH);
	return SUCCESS;
}

/**
 * @brief Reads from SPI connected device
 *
 * @param command to be written to SPI, specifying register to be written to
 * @param byte pointer in which data is read
 * @param count number of bytes to be read
 *
 * @return 
 */
uint8_t spi_read(uint8_t command, uint8_t* byte, uint8_t count)
{
	gpio_set_value(GPIO_SPI_CS, HIGH);
	gpio_set_value(GPIO_SPI_CS, LOW);
	rc_check = write(spi_fd, &command, 1);
	if(rc_check != 1)
	{
		printf("Register address write failed");
		return FAILURE;
//		exit(1);
	}

	rc_check = read(spi_fd, byte, count);
	if(rc_check != count)
	{
		printf("Data read from register failed");
		return FAILURE;
//	exit(1);
	}
	gpio_set_value(GPIO_SPI_CS, HIGH);
	
	return SUCCESS;
}
