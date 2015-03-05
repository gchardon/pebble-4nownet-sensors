#pragma once

#include <pebble.h>

// Settings
#define USE_TAP
#define MAX_NB_SENSORS 8
#define VALUE_STR_LEN sizeof("000.0")
#define LOCATION_STR_LEN 8
#ifdef PBL_COLOR
#define SENSOR_TEXT_COLOR GColorLightGray
#define SENSOR_LOW_COLOR GColorPictonBlue 
#define SENSOR_MID_COLOR GColorMediumAquamarine
#define SENSOR_HIGH_COLOR GColorYellow
// Thresholds are integer since we only use the integer part
// (i.e. sensor values are not rounded)
#define SENSOR_MID_TH 18  
#define SENSOR_HIGH_TH 21
#else
#define SENSOR_TEXT_COLOR GColorWhite
#endif

// Message Keys    
#define KEY_TIMESTAMP 0
#define KEY_SENSOR_VALUE 1
#define KEY_SENSOR_LOCATION 2
#define KEY_SENSOR_LIST 3
    
    