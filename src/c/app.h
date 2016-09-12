#include <inttypes.h>
#include <pebble.h>
#include <pebble-events/pebble-events.h>

// Configuration indexes
uint32_t CONFIG_WEATHER_APIKEY = 0;
uint32_t CONFIG_CLOCK_BGCOLOR = 1;
uint32_t CONFIG_CLOCK_COLOR = 2;
uint32_t  CONFIG_TIMES_UPDWEATHER = 3;
uint32_t CONFIG_TIMES_UPDSTEPS = 4;

// Week day names definitions
static const char *weekday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// Vibration definitions
static const uint32_t vibe_hour[] = {100, 100, 100};
static const uint32_t vibe_connect[] = {50, 100, 50, 100, 50};
static const uint32_t vibe_disconnect[] = {300, 100, 300, 100, 300};