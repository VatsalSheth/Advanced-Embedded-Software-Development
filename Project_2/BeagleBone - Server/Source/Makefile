objects = main.o spi_driver.o gpio.o nrf24L01.o map.o log.o
headers = ../Include/main.h ../Include/spi_driver.h ../Include/gpio.h ../Include/nrf24L01.h ../Include/map.h ../Include/log.h
 
CC = arm-linux-gnueabihf-gcc

project_2 : $(objects) $(headers)
	$(CC) -g -o project_2 $(objects) -lpthread -lrt -lm

gpio.o : gpio.c $(headers)
	$(CC) -c gpio.c

spi_driver.o : spi_driver.c $(headers)
	$(CC) -c spi_driver.c

nrf24L01.o : nrf24L01.c $(headers)
	$(CC) -c nrf24L01.c

map.o : map.c $(headers)
	$(CC) -c map.c

log.o : log.c $(headers)
	$(CC) -c log.c

main.o : main.c $(headers)
	$(CC) -c main.c

clean : 	
	rm -f project_2 $(objects)
