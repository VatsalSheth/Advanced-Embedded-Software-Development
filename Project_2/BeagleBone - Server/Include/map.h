/***********************************************************************************
* @map.h
* @This file contains dependent include files and variable declaration for map.c
*
* @author Vatsal Sheth & Sarthak Jain
************************************************************************************/

#ifndef _MAP_H
#define _MAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_DISTANCE_M	(1000)
#define RESOLUTION_M	(20)
#define N_NODES 	(MAX_DISTANCE_M/RESOLUTION_M)

#define FORWARD		(0b0001)
#define BACKWARD	(0b0010)
#define RIGHT		(0b0100)
#define LEFT		(0b1000)

#define ENGG_CIRCLE_LATI 	(40.0072704)
#define ENGG_CIRCLE_LONGI	(-105.2641923)

#define VERTEX_50_0_LATI	(40.0082206)
#define VERTEX_50_0_LONGI	(-105.2614143)
#define VERTEX_50_50_LATI	(40.0082206)
#define VERTEX_50_50_LONGI	(-105.2721049)
#define VERTEX_0_50_LATI	(40.0000974)
#define VERTEX_0_50_LONGI	(-105.2721049)
#define VERTEX_0_0_LATI		(40.0000974)
#define VERTEX_0_0_LONGI	(-105.2614143)

#define DEPT_PHY_LATI		(40.0082101)
#define DEPT_PHY_LONGI		(-105.2676004)
#define C4C_LATI		(40.0043916)
#define C4C_LONGI		(-105.2649423)

uint8_t direction, enable_target, mislead, reached;
uint8_t prev_index, cur_index, next_index;

struct map_coordinates
{
	int lati, longi;
}phy[29], c4c[19];

uint8_t direction_to_circle(float, float, float, float);
uint8_t direction_to_target(float, float, float, float, float, float);
void map_init(void);

#endif
