#include <pebble.h>

#define KEY_TIMESTAMP 0
#define KEY_SENSOR_VALUE 1
#define KEY_SENSOR_LOCATION 2
#define KEY_SENSOR_LIST 3
    
#define MAX_NB_SENSORS 8
#define VALUE_STR_LEN sizeof("000.0")
#define LOCATION_STR_LEN 16
    
// Used to store received sensor data
typedef struct SensorData {
    char value[VALUE_STR_LEN];
    char location[LOCATION_STR_LEN];
} SensorData;

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_sensor_layer;

static GFont s_time_font;
static GFont s_sensor_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static SensorData s_sensor_array[MAX_NB_SENSORS];
static unsigned char s_nb_sensors;

static void update_time() {
	// Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	// Create a long-lived buffer
	static char buffer[] = "00:00";
	// Write the current hours and minutes into the buffer
	if (clock_is_24h_style() == true) {
		//Use 24 hour format
		strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	} else {
		//Use 12 hour format
		strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
	}
	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
    s_nb_sensors = 0;
    APP_LOG(APP_LOG_LEVEL_INFO, "MAX_NB_SENSORS = %d", MAX_NB_SENSORS);
	// Create GBitmap, then set to created BitmapLayer
	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
	// Create time TextLayer
	s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorBlack);
	text_layer_set_text(s_time_layer, "00:00");
    
	// Create GFont
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
    
	// Apply to TextLayer
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    
	// Create temperature Layer
	s_sensor_layer = text_layer_create(GRect(0, 130, 144, 25));
	text_layer_set_background_color(s_sensor_layer, GColorClear);
	text_layer_set_text_color(s_sensor_layer, GColorWhite);
	text_layer_set_text_alignment(s_sensor_layer, GTextAlignmentCenter);
	text_layer_set_text(s_sensor_layer, "Loading...");
	// Create second custom font, apply it and add to Window
	s_sensor_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
	text_layer_set_font(s_sensor_layer, s_sensor_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_sensor_layer));
	// Make sure the time is displayed from the start
	update_time();
}

static void main_window_unload(Window *window) {
	// Unload GFont
	fonts_unload_custom_font(s_time_font);
    
	// Destroy GBitmap
	gbitmap_destroy(s_background_bitmap);
    
	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
    
	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
	// Destroy sensor elements
	text_layer_destroy(s_sensor_layer);
	fonts_unload_custom_font(s_sensor_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
    
	// Get sensor update every 30 minutes
	if (tick_time->tm_min % 30 == 0) {
		// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);
        
		// Add a key-value pair
		dict_write_uint8(iter, 0, 0);
        
		// Send the message!
		app_message_outbox_send();
	}
}

char* copy_next_token(char* src, char delim, char* dest, int max) {
    int l;
    char* p = src;
    while (*p  && (*p) != delim) p++;
    l = p - src;
    if (l >= max) l = max - 1;
    memcpy(dest, src, l);
    dest[l] = 0;
    return (*p && *(p+1)) ? (p+1) : 0;
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	// Store incoming information
    static char sensor_layer_buffer[32];
    char* pch;
    unsigned char sensor_idx = 0;
    
	// Read first item
	Tuple *t = dict_read_first(iterator);
    
	// For all items
	while (t != NULL) {
		// Which key was received?
		switch(t->key) {
			case KEY_SENSOR_VALUE:
                APP_LOG(APP_LOG_LEVEL_INFO, "Sensor Values are %s", t->value->cstring);
                pch = t->value->cstring;
                sensor_idx = 0;
                while (pch && sensor_idx < MAX_NB_SENSORS) {
                    pch = copy_next_token(pch, '|', s_sensor_array[sensor_idx++].value, VALUE_STR_LEN);
                    APP_LOG(APP_LOG_LEVEL_INFO, "Sensor #%d Value is %s", sensor_idx, s_sensor_array[sensor_idx-1].value);
                }
               APP_LOG(APP_LOG_LEVEL_INFO, "Found %d Values", sensor_idx);
			    break;
            case KEY_SENSOR_LOCATION:
                APP_LOG(APP_LOG_LEVEL_INFO, "Sensor Locations are %s", t->value->cstring);
                pch = t->value->cstring;
                sensor_idx = 0;
                while (pch && sensor_idx < MAX_NB_SENSORS) {
                    pch = copy_next_token(pch, '|', s_sensor_array[sensor_idx++].location, LOCATION_STR_LEN);
                    APP_LOG(APP_LOG_LEVEL_INFO, "Sensor #%d Location is %s", sensor_idx, s_sensor_array[sensor_idx-1].location);
                }
                APP_LOG(APP_LOG_LEVEL_INFO, "Found %d Locations", sensor_idx);
			    break;
			case KEY_TIMESTAMP:
			    APP_LOG(APP_LOG_LEVEL_INFO, "Sensor Timestamp is %d", (int)t->value->uint32);
			    break;
			default:
			    APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
			    break;
		}
        
		// Look for next item
		t = dict_read_next(iterator);
	}
    
	// Assemble full string and display
    if (sensor_idx > 0) {
        snprintf(sensor_layer_buffer, sizeof(sensor_layer_buffer), "%s@%s", s_sensor_array[0].value, s_sensor_array[0].location);
	    text_layer_set_text(s_sensor_layer, sensor_layer_buffer);
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
	// Create main Window element and assign to pointer
	s_main_window = window_create();
    
	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
        .unload = main_window_unload
    });
    
	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);
    
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
    
	// Open AppMessage
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}