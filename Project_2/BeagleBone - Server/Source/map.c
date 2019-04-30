#include "../Include/map.h"

//uint8_t graph[N_NODES][N_NODES] = {0};

//void main()
//{
//	printf("%x\n", direction_to_circle(VERTEX_50_0_LATI, VERTEX_50_0_LONGI, 40.0072704, -105.2649423));
//}


/**
 * @brief Returns direction to hard-coded location, Engineering circle. Does so by taking floating inputs
 * and computing the direction to be travelled in by taking difference between previous and current locations and 
 * judging based on their sign. 
 *
 * @param prev_lati
 * @param prev_longi
 * @param cur_lati
 * @param cur_longi
 *
 * @return direction
 */
uint8_t direction_to_circle(float prev_lati, float prev_longi, float cur_lati, float cur_longi)
{
	mislead = 0;
	uint8_t only_lati = 0, only_longi = 0;
	prev_lati = N_NODES - (int)((VERTEX_50_0_LATI - prev_lati)*(111320/RESOLUTION_M) + 0.5);
	prev_longi = (int)((VERTEX_50_0_LONGI - prev_longi)*(111320/RESOLUTION_M) + 0.5);
	cur_lati = N_NODES - (int)((VERTEX_50_0_LATI - cur_lati)*(111320/RESOLUTION_M) + 0.5);
	cur_longi = (int)((VERTEX_50_0_LONGI - cur_longi)*(111320/RESOLUTION_M) + 0.5);

	int circle_lati =  N_NODES - (int)((VERTEX_50_0_LATI - ENGG_CIRCLE_LATI)*(111320/RESOLUTION_M) + 0.5);
	int circle_longi =  (int)((VERTEX_50_0_LONGI - ENGG_CIRCLE_LONGI)*(111320/RESOLUTION_M) + 0.5);
	printf("lati:%d longi:%d\n", circle_lati, circle_longi);

	if(circle_lati - cur_lati == 0)
	{
		only_longi = 1;
	}
	if(circle_longi - cur_longi == 0)
	{
		only_lati = 1;
	}
	
	if(only_longi && only_lati)
	{
		enable_target = 1;
	}

	if(!only_longi)
	{
		if(circle_lati - cur_lati > 0)
		{
			if((circle_lati - prev_lati > 0) || (prev_lati - cur_lati > 0))
			{
				direction |= RIGHT;
				if(prev_lati - cur_lati > 0)
				{
					mislead = 1;
				}
			}
		}
		else 
		{
			if((circle_lati - prev_lati < 0) || (prev_lati - cur_lati < 0))
			{
			 	direction |= LEFT;
				if(prev_lati - cur_lati < 0)
				{
					mislead = 1;
				}
			}
		}	
	}

	if(!only_lati)
	{
		if(circle_longi - cur_longi > 0)
		{
			if((circle_longi - prev_longi > 0) || (prev_longi - cur_longi > 0))
			{
				direction |= FORWARD;
				if(prev_longi - cur_longi > 0)
				{
					mislead = 1;
				}
			}
		}
		else 
		{
			if((circle_longi - prev_longi < 0) || (prev_longi - cur_longi < 0))
			{
			 	direction |= BACKWARD;
				if(prev_longi - cur_longi < 0)
				{
					mislead = 1;
				}
			}
		}	
	}
	return (direction | (mislead<<4) | (reached<<5));
}

/**
 * @brief Computes direction to target, provided previous location, current location and target location
 *
 * @param prev_lati
 * @param prev_longi
 * @param cur_lati
 * @param cur_longi
 * @param target_lati
 * @param target_longi
 *
 * @return direction in terms of FORWARD, BACKWARD, LEFT, RIGHT 
 */
uint8_t direction_to_target(float prev_lati, float prev_longi, float cur_lati, float cur_longi, float target_lati, float target_longi)
{
	mislead = 0;
	uint8_t only_lati = 0, only_longi = 0;
	
	prev_lati = N_NODES - (int)((VERTEX_50_0_LATI - prev_lati)*(111320/RESOLUTION_M) + 0.5);
	prev_longi = (int)((VERTEX_50_0_LONGI - prev_longi)*(111320/RESOLUTION_M) + 0.5);
	cur_lati = N_NODES - (int)((VERTEX_50_0_LATI - cur_lati)*(111320/RESOLUTION_M) + 0.5);
	cur_longi = (int)((VERTEX_50_0_LONGI - cur_longi)*(111320/RESOLUTION_M) + 0.5);

	target_lati =  N_NODES - (int)((VERTEX_50_0_LATI - target_lati)*(111320/RESOLUTION_M) + 0.5);
	target_longi =  (int)((VERTEX_50_0_LONGI - target_longi)*(111320/RESOLUTION_M) + 0.5);
//	printf("lati:%d longi:%d\n", circle_lati, circle_longi);

	if((target_lati == 50) && (target_longi == 38))
	{
		if((cur_lati == phy[cur_index].lati) && (cur_longi == phy[cur_index].longi))
		{
			next_index = cur_index + 1;
			target_lati = phy[next_index].lati;
			target_longi = phy[next_index].longi;
			cur_index++;
		}
		else
		{
			mislead = 1;
			target_lati = phy[cur_index].lati;
			target_longi = phy[cur_index].longi;
		}

		if(target_lati - cur_lati == 0)
		{
			only_longi = 1;
		}
		if(target_longi - cur_longi == 0)
		{
			only_lati = 1;
		}

		if(only_longi && only_lati)
		{
			reached = 1;
		}

		if(!only_longi)
		{
			if(target_lati - cur_lati > 0)
			{
				if((target_lati - prev_lati > 0) || (prev_lati - cur_lati > 0))
				{
					direction |= RIGHT;
					if(prev_lati - cur_lati > 0)
					{
						mislead = 1;
					}
				}
			}
			else 
			{
				if((target_lati - prev_lati < 0) || (prev_lati - cur_lati < 0))
				{
		 			direction |= LEFT;
					if(prev_lati - cur_lati < 0)
					{
						mislead = 1;
					}
				}
			}	
		}

		if(!only_lati)
		{
			if(target_longi - cur_longi > 0)
			{
				if((target_longi - prev_longi > 0) || (prev_longi - cur_longi > 0))
				{
					direction |= FORWARD;
					if(prev_longi - cur_longi > 0)
					{
						mislead = 1;
					}
				}
			}
			else 
			{
				if((target_longi - prev_longi < 0) || (prev_longi - cur_longi < 0))
				{
				 	direction |= BACKWARD;
					if(prev_longi - cur_longi < 0)
					{
						mislead = 1;
					}
				}
			}	
		}
		return (direction | (mislead<<4) | (reached<<5));
	}
	else if((target_lati == 31) && (target_longi == 19))
	{
		if((cur_lati == c4c[cur_index].lati) && (cur_longi == c4c[cur_index].longi))
		{
			next_index = cur_index + 1;
			target_lati = c4c[next_index].lati;
			target_longi = c4c[next_index].longi;
			cur_index++;
		}
		else
		{
			mislead = 1;
			target_lati = c4c[cur_index].lati;
			target_longi = c4c[cur_index].longi;
		}

		if(target_lati - cur_lati == 0)
		{
			only_longi = 1;
		}
		if(target_longi - cur_longi == 0)
		{
			only_lati = 1;
		}
	
		if(only_longi && only_lati)
		{
			reached = 1;
		}

		if(!only_longi)
		{
			if(target_lati - cur_lati > 0)
			{
				if((target_lati - prev_lati > 0) || (prev_lati - cur_lati > 0))
				{
					direction |= RIGHT;
					if(prev_lati - cur_lati > 0)
					{
						mislead = 1;
					}
				}
			}
			else 
			{
				if((target_lati - prev_lati < 0) || (prev_lati - cur_lati < 0))
				{
			 		direction |= LEFT;
					if(prev_lati - cur_lati < 0)
					{
						mislead = 1;
					}
				}
			}	
		}

		if(!only_lati)
		{
			if(target_longi - cur_longi > 0)
			{
				if((target_longi - prev_longi > 0) || (prev_longi - cur_longi > 0))
				{	
					direction |= FORWARD;
					if(prev_longi - cur_longi > 0)
					{
						mislead = 1;
					}
				}
			}
			else 
			{
				if((target_longi - prev_longi < 0) || (prev_longi - cur_longi < 0))
				{
				 	direction |= BACKWARD;
					if(prev_longi - cur_longi < 0)
					{
						mislead = 1;
					}
				}
			}	
		}
		return (direction | (mislead<<4) | (reached<<5));
	}
}

/**
 * @brief Initialized a map for two paths, one to C4C, one to Physics Department
 */
void map_init()
{
	int i;
	phy[0].lati = 45;
	phy[0].longi = 15;

	c4c[0].lati = 45;
	c4c[1].longi = 15;

	for(i=1;i<=12;i++)
	{
		phy[i].longi = phy[i-1].longi + 1;
	}
	for(i=13;i<=17;i++)
	{
		phy[i].lati = phy[i-1].lati + 1;
	}
	for(i=18;i<=29;i++)
	{
		phy[i].longi = phy[i-1].longi + 1;
	}

	for(i=1;i<=14;i++)
	{
		phy[i].lati = phy[i-1].lati - 1;
	}
	for(i=15;i<=19;i++)
	{
		phy[i].longi = phy[i-1].longi + 1;
	}
}
