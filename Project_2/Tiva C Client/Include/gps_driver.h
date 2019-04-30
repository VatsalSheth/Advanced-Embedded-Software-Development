/**
 * File: gps_driver.h
 * Author: Vatsal Sheth & Sarthak Jain
 * Description: This file contains required include and declarations of variables and functions used by gps_driver.c
 * Date: 4/29/2019
 * Reference: Arduino TinyGPS C++ library
 */


#ifndef INCLUDE_GPS_DRIVER_H_
#define INCLUDE_GPS_DRIVER_H_


#include <math.h>
#include <time.h>
#include "basic.h"

typedef char boole;
typedef unsigned char byte;
#define false 0
#define true 1

#define PI 3.14159265
#define TWO_PI 2*PI

#define sq(x) ((x)*(x))

#define GPRMC_TERM   "GPRMC"
#define GPGGA_TERM   "GPGGA"

#define GPS_INVALID_F_ANGLE 1000.0
#define GPS_INVALID_F_ALTITUDE 1000000.0
#define GPS_INVALID_F_SPEED -1.0


#define GPS_VERSION 12 // software version of this library
#define GPS_MPH_PER_KNOT 1.15077945
#define GPS_MPS_PER_KNOT 0.51444444
#define GPS_KMPH_PER_KNOT 1.852
#define GPS_MILES_PER_METER 0.00062137112
#define GPS_KM_PER_METER 0.001

//#define GPS_NO_STATS

enum
{
    GPS_INVALID_AGE = 0xFFFFFFFF,
    GPS_INVALID_ANGLE = 999999999,
    GPS_INVALID_ALTITUDE = 999999999,
    GPS_INVALID_DATE = 0,
    GPS_INVALID_TIME = 0xFFFFFFFF,
    GPS_INVALID_SPEED = 999999999,
    GPS_INVALID_FIX_TIME = 0xFFFFFFFF,
    GPS_INVALID_SATELLITES = 0xFF,
    GPS_INVALID_HDOP = 0xFFFFFFFF
};

// process one character received from GPS
boole encode(char c);

// lat/long in hundred thousandths of a degree and age of fix in milliseconds
void gps_get_position(long *latitude, long *longitude, unsigned long *fix_age);

// date as ddmmyy, time as hhmmsscc, and age in milliseconds
void gps_get_datetime(unsigned long *date, unsigned long *time, unsigned long *age);

// satellites used in last full GPGGA sentence
inline unsigned short gps_satellites(void);

// horizontal dilution of precision in 100ths
inline unsigned long gps_hdop(void);


boole gps_encode(char);

void gps_f_get_position(float *latitude, float *longitude, unsigned long *fix_age);
void gps_crack_datetime(int *year, byte *month, byte *day, byte *hour, byte *minute, byte *second, byte *hundredths, unsigned long *fix_age);
float gps_f_altitude();
float gps_f_course();
float gps_f_speed_knots();
float gps_f_speed_mph();
float gps_f_speed_mps();
float gps_f_speed_kmph();

static int library_version() { return GPS_VERSION; }

static float gps_distance_between (float lat1, float long1, float lat2, float long2);
extern float gps_course_to (float lat1, float long1, float lat2, float long2);
static const char *gps_cardinal(float course);

#ifndef GPS_NO_STATS
void gps_stats(unsigned long *chars, unsigned short *good_sentences, unsigned short *failed_cs);
#endif

enum {
    GPS_SENTENCE_GPGGA,
    GPS_SENTENCE_GPRMC,
    GPS_SENTENCE_OTHER
};

// internal utilities
int from_hex(char a);
unsigned long gps_parse_decimal();
unsigned long gps_parse_degrees();
boole gps_term_complete();
boole gpsisdigit(char c);
long gpsatol(const char *str);
int gpsstrcmp(const char *str1, const char *str2);


#endif /* INCLUDE_GPS_DRIVER_H_ */
