#pragma once

#include <pebble.h>

// Settings
#define USE_TAP
#define MAX_NB_SENSORS 8
#define VALUE_STR_LEN sizeof("000.0")
#define LOCATION_STR_LEN 8

// Message Keys    
#define KEY_TIMESTAMP 0
#define KEY_SENSOR_VALUE 1
#define KEY_SENSOR_LOCATION 2
#define KEY_SENSOR_LIST 3
    
    