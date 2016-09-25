#include <inttypes.h>
#include <pebble.h>
#include <pebble-events/pebble-events.h>

// Configuration indexes
#define CONFIG_CLOCK_BGCOLOR         0
#define CONFIG_CLOCK_COLOR           1

// Week day names definitions
static const char *weekday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// Vibration definitions
static const uint32_t vibe_hour[] = {100, 100, 100};
static const uint32_t vibe_connect[] = {50, 100, 50, 100, 50};
static const uint32_t vibe_disconnect[] = {300, 100, 300, 100, 300};