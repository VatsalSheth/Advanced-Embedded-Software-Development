/***********************************************************************************
* @main.c
* @This file contains main(), with thread creation of a logger thread and an interrupt
* polling thread. It also initializes two timers.
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#include "../Include/main.h"

/**
 * @brief Configures the NRF module, along with a self-test in it. Initializes timers
 * and the map to be used for navigation.
 *
 * @param argc
 * @param argv[]
 *
 * @return 
 */
int main(int argc, char *argv[])
{
	uint8_t temp;
//	gpio_export(GPIO_SPI_CS);
	
	ConfigureNRF();	
	time_timer_init();
	watchdog_timer_init();
	map_init();
	
	int rc = pthread_create(&int_th, (void *)0, NRF_ISR, (void *)0);
	if(rc != 0)
		handle_error("Error in creating NRF interrupt thread")

	rc = pthread_create(&log_th, (void *)0, logger_func, (void *)0);
	if(rc != 0)
		handle_error("Error in creating logger thread")

/*	rx_packet.lati = 12.34;
	rx_packet.longi = 56.78;
	rx_packet.seconds = 15;
	rx_packet.status = 10;*/

	uint8_t arr[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'A', 'B', 'C'};
//	uint8_t local_byte = 0x0a;
//	temp = NRF_write(W_REGISTER(CONFIG_REG), &local_byte, 1);	
//    	temp = NRF_write(W_TX_PAYLOAD, (uint8_t*)arr, sizeof(struct tiva_2_bbg));
	set_CE();
	
	pthread_join(int_th, NULL);
	close(spi_fd);
	return 0;
}

/**
 * @brief Polls the GPIO pins for interrupts. Three types of interrupts may occur, 
 * one for Rx, Tx and for Maximum Retransmissions. Entire controlling logic on the BBG
 * side is given in the ISR. Mapping functions are called within the Controlling Logic 
 * Block. Rx and Tx oxxurs through use of a packed structure with te fields, float latitude, 
 * float longitude, uint8 status and uint32 time.
 *
 *
 * @param threadp
 *
 * @return 
 */
void* NRF_ISR(void* threadp)
{
	struct pollfd fdset[1];
	struct log_msg data_log; 
	int nfds, rc_int, status;
	char value[5];
	uint8_t local_byte, allow;
	backup_lati = 40.0072073;
	backup_longi = -105.2637518;

//	gpio_export(NRF_SPI_IRQ);
	gpio_dir(NRF_SPI_IRQ, "in");
	gpio_edge(NRF_SPI_IRQ, "both");
	gpio_nrf_fd = gpio_open(NRF_SPI_IRQ, "value");

	nfds = 1;
		
//	enable_target = 1;	//FOR TESTING ONLY
	
	while(1)
	{
		memset((void*)fdset, 0, sizeof(fdset));
		
		fdset[0].fd = gpio_nrf_fd; 
		fdset[0].events = POLLPRI;
		
		rc_int = poll(fdset, nfds, 1000);
		if(rc_int < 0)
			handle_error("poll")
		else if(rc_int == 0)
		{
			continue;
		}
			
		if (fdset[0].revents & POLLPRI) 
		{
			lseek(fdset[0].fd, 0, SEEK_SET);
			read(fdset[0].fd, value, 5);
			
			status = NRF_read(R_REGISTER(STATUS_REG), byte, 1);
printf("Interrupt received, STATUS REG: %x\n", byte[0]);
			if(*byte & RX_FIFO_IRQ)
			{
				status = NRF_read(R_RX_PAYLOAD, (uint8_t *)&rx_packet, sizeof(struct tiva_2_bbg));
				if(status != SUCCESS)
					handle_error("Error receiving packet from TIVA")
//					printf("\nRECEIVED:%x %x %x %x\n", byte[0], byte[1], byte[2], byte[3]);
				printf("\nLat: %f, Long: %f, Status: %u, Time: %d\n", rx_packet.lati, rx_packet.longi, rx_packet.status, rx_packet.seconds);

				*byte |= RX_FIFO_IRQ;
				status = NRF_write(W_REGISTER(STATUS_REG), byte, 1);
printf("RX FIFO IRQ detected\n");
				if(status != SUCCESS)
					handle_error("Error clearing Receiving FIFO interrupt");
				
				if(rx_packet.status & TIME_EXC_RX)
				{
					local.hours = rx_packet.lati;
					tiva_t.hours = rx_packet.lati;
					local.minutes = rx_packet.longi;
					tiva_t.minutes = rx_packet.longi;
					local.seconds = rx_packet.seconds;
					tiva_t.seconds = rx_packet.seconds;
					timer_settime(time_timer_id, 0, &time_trigger, NULL);
				
					clear_CE();	
				
					local_byte = 0x0a;
					rc_int = NRF_write(W_REGISTER(CONFIG_REG), &local_byte, 1);	
					if(rc_int != SUCCESS)
						handle_error("Unable to convert to Rx mode")
					tx_packet.status = 0b01000000;
    					rc_int = NRF_write(W_TX_PAYLOAD, (uint8_t *)&tx_packet, sizeof(struct bbg_2_tiva));
					if(rc_int != SUCCESS)
						handle_error("Unable to transmit")
					set_CE();
				}
				else
				{
					update_tiva_time(rx_packet.seconds);
					if(compare_time())
					{
						allow = 1;
					}
					else 
					{
						if(rx_packet.status & N_IGNORE_RX)
						{
							allow = 1;
						}
						else allow = 0;
					}
					if(allow)
					{
						if(!(rx_packet.status & CONTINUE_RX))	
						{
							clear_CE();
							local_byte = 0x0a;
							rc_int = NRF_write(W_REGISTER(CONFIG_REG), &local_byte, 1);	
							if(rc_int != SUCCESS)
								handle_error("Unable to convert to Tx mode")
							if(rx_packet.status & GPS_RX)
							{
								data_log.status |= GPS_RX;
								tx_packet.lati = NO_VALID_DATA;
								tx_packet.longi = NO_VALID_DATA;
							}
							if(!(rx_packet.status & ACCEL_RX))
							{
								if(rx_packet.status & ACCIDENT_RX)
								{
									data_log.status |= ACCIDENT_RX;
									tx_packet.lati = DEPT_PHY_LATI;
									tx_packet.longi = DEPT_PHY_LONGI;
								}
								else
								{
									tx_packet.lati = C4C_LATI;
									tx_packet.longi = C4C_LONGI;
								}
							}
							else 
							{
								data_log.status |= ACCEL_RX;
								tx_packet.lati = NO_VALID_DATA;
								tx_packet.longi = NO_VALID_DATA;
							}
							if(!enable_target)
							{
								tx_packet.status = direction_to_circle(backup_lati, backup_longi, rx_packet.lati, rx_packet.longi);
							}
							else
							{
								tx_packet.status = direction_to_target(backup_lati, backup_longi, rx_packet.lati, rx_packet.longi, tx_packet.lati, tx_packet.longi);
							}

							printf("Direction to travel in: %x\n", tx_packet.status);	//TESTING ONLY

    							rc_int = NRF_write(W_TX_PAYLOAD, (uint8_t *)&tx_packet, sizeof(struct bbg_2_tiva));
							if(rc_int != SUCCESS)
								handle_error("Unable to transmit")

							set_CE();
							backup_lati = rx_packet.lati;
							backup_longi = rx_packet.longi;
							data_log.lat = rx_packet.lati;
							data_log.lon = rx_packet.longi;
							data_log.id = POLL_IRQ;
							mq_send(queue_fd, (char*)&data_log, sizeof(struct log_msg), 0);
						}
						else
						{
							//buffer data in Rx buffer
						}
					}
				}
				timer_settime(watchdog_timer_id, 0, &watchdog_trigger, NULL);
			}
			else if(*byte & TX_FIFO_IRQ)
			{

				clear_CE();	
				
				*byte |= TX_FIFO_IRQ;
				status = NRF_write(W_REGISTER(STATUS_REG), byte, 1);
printf("Tx FIFO IRQ\n");
				if(status != SUCCESS)
					handle_error("Error clearing Transmission FIFO interrupt");
				
				local_byte = 0x0b;
				rc_int = NRF_write(W_REGISTER(CONFIG_REG), &local_byte, 1);	
				if(rc_int != SUCCESS)
					handle_error("Unable to convert to Rx mode")
				set_CE();
				timer_settime(watchdog_timer_id, 0, &watchdog_trigger, NULL);
			}
			else if(*byte & MAX_RT_IRQ)
			{
				*byte |= MAX_RT_IRQ;
				status = NRF_write(W_REGISTER(STATUS_REG), byte, 1);
printf("MAX IRQ detected\n");
				if(status != SUCCESS)
					handle_error("Error clearing Max Retransmission interrupt");
			}	
			
		}
	}
}

/**
 * @brief Watchdog timer, for namesake. 
 * Calls a handler every 7 seconds to revert in Rx mode, in case it is stuck somewhere.
 * Timer is re-initialized every time an interrupt comes.
 */
void watchdog_timer_init()
{	
	struct sigevent sev;
//	struct itimerspec trigger;

	memset(&sev, 0, sizeof(struct sigevent));
    	memset(&watchdog_trigger, 0, sizeof(struct itimerspec));

    	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = watchdog_timer_handle;
//	sev.sigev_value.sival_int = pin;

	timer_create(CLOCK_REALTIME, &sev, &watchdog_timer_id);

	watchdog_trigger.it_interval.tv_sec = 7;
	watchdog_trigger.it_value.tv_sec = 7;

//	timer_settime(time_timer_id, 0, &trigger, NULL);
}

/**
 * @brief Watchdog Timer handler
 *
 * @param sv
 */
void watchdog_timer_handle(union sigval sv)
{
	clear_CE();
	uint8_t local_byte = 0x0b;
	uint8_t rc;
	rc = NRF_write(W_REGISTER(CONFIG_REG), &local_byte, 1);	
	if(rc != SUCCESS)
		handle_error("Unable to convert to Rx mode")
	set_CE();
	printf("\nIn timer after 7 seconds\n");	//DEBUG PRINTF
}

/**
 * @brief Initialized a timer called every second to keep a track of time on BBG
 */
void time_timer_init()
{	
	struct sigevent sev;
//	struct itimerspec trigger;

	memset(&sev, 0, sizeof(struct sigevent));
    	memset(&time_trigger, 0, sizeof(struct itimerspec));

    	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = time_timer_handle;
//	sev.sigev_value.sival_int = pin;

	timer_create(CLOCK_REALTIME, &sev, &time_timer_id);

	time_trigger.it_interval.tv_sec = 1;
	time_trigger.it_value.tv_sec = 1;

//	timer_settime(time_timer_id, 0, &trigger, NULL);
}

void time_timer_handle(union sigval sv)
{
	if(local.seconds == 59)
    	{
        	if(local.minutes == 59)
        	{
            		if(local.hours == 23)
            		{
                		local.hours = 0;
            		}
            		else
            		{
                		local.hours += 1;
            		}
            		local.minutes = 0;
        	}
        	else
        	{
            		local.minutes += 1;
        	}
        	local.seconds = 0;
    	}
    	else
    	{
        	local.seconds += 1;
    	}
}

/**
 * @brief Updates TIVA time based on argument in seconds
 *
 * @param add_seconds Count of seconds obtained from TIVA. Adds to initial time obtained from TIVA
 * to keep an overall count of time on TIVA
 */
void update_tiva_time(uint32_t add_seconds)
{
	struct log_msg data_log_from_ISR; 
	uint8_t add_hours = add_seconds/3600;
      	uint8_t add_minutes = (add_seconds%3600)/60;
	add_seconds = ((uint32_t)add_seconds%3600)%60;

	if(tiva_t.seconds+add_seconds > 59)
    	{
        	if(tiva_t.minutes+add_minutes > 59)
        	{
            		if(tiva_t.hours+add_hours > 23)
            		{
                		tiva_t.hours = (tiva_t.hours + add_hours)%24;
            		}
            		else
            		{
                		tiva_t.hours += add_hours;
            		}
            		tiva_t.minutes = (tiva_t.minutes + add_minutes)%60;
        	}
        	else
        	{
            		tiva_t.minutes += add_minutes;
        	}
        	tiva_t.seconds = (tiva_t.seconds + add_seconds)%60;
    	}
    	else
    	{
        	tiva_t.seconds += add_seconds;
    	}
	data_log_from_ISR.hours = tiva_t.hours;
	data_log_from_ISR.minutes = tiva_t.minutes;
	data_log_from_ISR.seconds = tiva_t.seconds;
	data_log_from_ISR.id = TIMER_ISR;
	mq_send(queue_fd, (char*)&data_log_from_ISR, sizeof(struct log_msg), 0);
}

/**
 * @brief compares local time on BBG and time obtained from TIVA
 * 
 *
 * @return Return failure if difference is greater than 30 seconds
 */
uint8_t compare_time()
{
	if((local.hours == tiva_t.hours) && (local.minutes == tiva_t.minutes))
	{
		if(((((int)local.seconds) - ((int)tiva_t.seconds)) > 30) ||
	       	  ((((int)local.seconds) - ((int)tiva_t.seconds)) < -30))
		{
			return OUT_RANGE;
		}
		else return IN_RANGE;
	}
	else return OUT_RANGE;	
}
