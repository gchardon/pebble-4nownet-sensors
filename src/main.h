#pragma once

#include <pebble.h>

//
// Settings
//
#define USE_TAP
#define MAX_NB_SENSORS 8
#define LOCATION_STR_LEN 8

#ifdef PBL_COLOR
#define SENSOR_NA_COLOR GColorDarkGray
#define SENSOR_TEXT_COLOR GColorWhite
#else
#define SENSOR_TEXT_COLOR GColorWhite
#define SENSOR_NA_COLOR SENSOR_TEXT_COLOR
#endif

//
// Constants
//
#define NB_SENSOR_LAYERS 4
#define SENSOR_BUFFER_LEN 16

// Message Keys    
#define KEY_TIMESTAMP 0
#define KEY_SENSOR_VALUE 1
#define KEY_SENSOR_TYPE 2
#define KEY_SENSOR_LOCATION 3
#define KEY_SENSOR_LIST 4
    
//    
// Types and helpers
//

// Used to store received sensor data
typedef struct {
    int value;
    char unit;
    char location[LOCATION_STR_LEN];
} SensorData;

#ifdef PBL_COLOR
// Sensor threshold and colors
typedef struct {
    int threshold;
    GColor color;
} SensorColorThreshold;
#endif

