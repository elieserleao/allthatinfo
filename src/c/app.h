#include <inttypes.h>
#include <pebble.h>
#include <pebble-events/pebble-events.h>

// Configuration indexes
#define CONFIG_WEATHER_APIKEY    	   0
#define CONFIG_CLOCK_BGCOLOR         1
#define CONFIG_CLOCK_COLOR           2
#define CONFIG_TIMES_UPDWEATHER      3
#define CONFIG_TIMES_UPDSTEPS        4
#define CONFIG_HOURLY_VIBRATE        5
#define CONFIG_HOURLY_VIBRATE_START  7
#define CONFIG_HOURLY_VIBRATE_STOP   8
#define CONFIG_WEATHER_UNIT          9

// Week day names definitions
static const char *weekday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// Vibration definitions
static const uint32_t vibe_hour[] = {100, 100, 100};
static const uint32_t vibe_connect[] = {50, 100, 50, 100, 50};
static const uint32_t vibe_disconnect[] = {300, 100, 300, 100, 300};