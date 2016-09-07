#include <pebble.h>
#include <pebble-events/pebble-events.h>
//Teste
static Window *s_main_window;
static Layer *s_bgtime_layer;
static Layer *s_batt_layer;
static TextLayer *s_city_layer;
static TextLayer *s_connection_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_temp_layer;
static TextLayer *s_hum_layer;
static TextLayer *s_wind_layer;
static TextLayer *s_cond_layer;
static TextLayer *s_sunset_layer;
static TextLayer *s_steps_layer;


static GBitmap *s_bitmap_noblt;
static BitmapLayer *s_bitmap_noblt_layer;

static GBitmap *s_bitmap_sunrise;
static BitmapLayer *s_bitmap_sunrise_layer;
static GBitmap *s_bitmap_sunset;
static BitmapLayer *s_bitmap_sunset_layer;

static GBitmap *s_bitmap_owmlogo;
static GBitmap *s_bitmap_wulogo;
static BitmapLayer *s_bitmap_wlogo_layer;

static GBitmap *s_bitmap_hum;
static BitmapLayer *s_bitmap_hum_layer;
static GBitmap *s_bitmap_windms;
static BitmapLayer *s_bitmap_windms_layer;
static GBitmap *s_bitmap_wind;
static BitmapLayer *s_bitmap_wind_layer;

static GBitmap *s_bitmap_run;
static BitmapLayer *s_bitmap_run_layer;

static BatteryChargeState *charge_state;

static const char *weekday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const uint32_t vibe_hour[] = {100, 100, 100};
static const uint32_t vibe_connect[] = {50, 100, 50, 100, 50};
static const uint32_t vibe_disconnect[] = {300, 100, 300, 100, 300};

static char apikey[32];

uint32_t CONFIG_WEATHER_APIKEY = 0;

static void bgtime_update_proc(Layer *layer, GContext *ctx) {
  GRect layer_bounds = layer_get_bounds(layer);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  GRect rect_bounds = GRect(0, 22, layer_bounds.size.w, 35);
  
  // Draw a rectangle
  graphics_draw_rect(ctx, rect_bounds);
  
  // Fill a rectangle with rounded corners
  int corner_radius = 0;
  
  graphics_fill_rect(ctx, rect_bounds, corner_radius, GCornersAll);
}

static void sbatt_update_proc(Layer *layer, GContext *ctx) {
  GRect layer_bounds = layer_get_bounds(layer);
  int y_start_point = 19;
    
  if (!charge_state){
    *charge_state = battery_state_service_peek();
  }  
  
  int sz = (layer_bounds.size.w * charge_state->charge_percent) / 100;
  
  graphics_context_set_fill_color(ctx, GColorLightGray);
  GRect rect_fill = GRect(0, y_start_point, layer_bounds.size.w, 3);
  
  graphics_draw_rect(ctx, rect_fill);
  graphics_fill_rect(ctx, rect_fill, 0, GCornerNone);
  
  GRect *rect_batt = (GRect*)malloc(sizeof(GRect));
  
  if(charge_state->is_charging){
    *rect_batt = GRect(0, y_start_point, layer_bounds.size.w, 3);
    graphics_context_set_fill_color(ctx, GColorYellow);
  }else{
    *rect_batt = GRect(0, y_start_point, sz, 3);
    
    graphics_context_set_fill_color(ctx, GColorGreen);
    if(charge_state->charge_percent <= 20){
      graphics_context_set_fill_color(ctx, GColorRed);
    }
  }
  
  graphics_draw_rect(ctx, *rect_batt);
  int corner_radius = 0;
  graphics_fill_rect(ctx, *rect_batt, corner_radius, GCornersAll);
}

static void loadconfig(){
  if(persist_exists(CONFIG_WEATHER_APIKEY)){
    persist_read_string(CONFIG_WEATHER_APIKEY, apikey, sizeof(apikey));
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Load APIKey: %s", apikey);
  }
}

static void request_weather(void *context) {
  DictionaryIterator *iter;
  AppMessageResult result = app_message_outbox_begin(&iter);

  if(result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error creating outbox message: %d", result);
    return;
  }
  
  dict_write_cstring(iter, MESSAGE_KEY_WEATHER_REQUEST, apikey);
  
  result = app_message_outbox_send();
  if(result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to send outbox message: %d", result);
    return;
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *reply_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_REPLY);
  if(reply_tuple) {
    Tuple *temp_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_TEMP);
    if(temp_tuple){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "TEMP: %s", temp_tuple->value->cstring);
      text_layer_set_text(s_temp_layer, temp_tuple->value->cstring);
    }
  
    Tuple *cond_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_COND);
    if(cond_tuple){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "COND: %s", cond_tuple->value->cstring);
      text_layer_set_text(s_cond_layer, cond_tuple->value->cstring);
    }
  
    Tuple *city_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_CITY);
    if(city_tuple){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "CITY: %s", city_tuple->value->cstring);
      text_layer_set_text(s_city_layer, city_tuple->value->cstring);
    }
  
    Tuple *sun_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_SUN);
    if(sun_tuple){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "SUN: %s", sun_tuple->value->cstring);
      text_layer_set_text(s_sunset_layer, sun_tuple->value->cstring);
    }
  
    Tuple *hum_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_HUM);
    if(hum_tuple){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "HUM: %s", hum_tuple->value->cstring);
      text_layer_set_text(s_hum_layer, hum_tuple->value->cstring);
    }
  
    Tuple *wind_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_WIND);
    if(wind_tuple){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "WIND: %s", wind_tuple->value->cstring);
      text_layer_set_text(s_wind_layer, wind_tuple->value->cstring);
    }
  
    Tuple *owm_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_OWM);
    if(owm_tuple){
      bitmap_layer_set_bitmap(s_bitmap_wlogo_layer, s_bitmap_owmlogo);
    }
    
    Tuple *wu_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_WU);
    if(wu_tuple){
      bitmap_layer_set_bitmap(s_bitmap_wlogo_layer, s_bitmap_wulogo);
    }
  }
  
  Tuple *api_tuple = dict_find(iter, MESSAGE_KEY_WEATHER_APIKEY);
  if(api_tuple){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "APIKEY: %s", api_tuple->value->cstring);
    persist_write_string(CONFIG_WEATHER_APIKEY, api_tuple->value->cstring);
    
    loadconfig();
    
    request_weather(NULL);
  }
}

static void handle_battery(BatteryChargeState state) {
  *charge_state = state;
  
  layer_mark_dirty(s_batt_layer);
}

static void refresh_steps(){
  #if defined(PBL_HEALTH)
  
  HealthMetric metric = HealthMetricStepCount;
  time_t start = time_start_of_today();
  time_t end = time(NULL);
  
  // Check the metric has data available for today
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
    start, end);
  
  if(mask & HealthServiceAccessibilityMaskAvailable) {
    char *cursteps = (char*)malloc(sizeof(char*[16]));
    
    snprintf(cursteps, sizeof(char*[16]), "%d", (int)health_service_sum_today(metric));
    text_layer_set_text(s_steps_layer, cursteps);
  } else {
    // No data recorded yet today
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
  }
  
  #else
    APP_LOG(APP_LOG_LEVEL_WARNING, "Pebble Health not available.");
  #endif
}

static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
  // Needs to be static because it's used by the system later.
  static char s_time_text[] = "00:00";
  static char s_date_text[] = "24 September";
  static char s_finaldate_text[] = "Sun, 24 September";                                         
                                         
  strftime(s_time_text, sizeof(s_time_text), "%I:%M", tick_time);
  strftime(s_date_text, sizeof(char*[32]), "%d %B", tick_time);
  
  snprintf(s_finaldate_text, sizeof(char*[32]), "%s, %s", weekday[tick_time->tm_wday], s_date_text);
  
  text_layer_set_text(s_time_layer, s_time_text);
  text_layer_set_text(s_date_layer, s_finaldate_text);
  
  if(tick_time->tm_min % 5 == 0){
    refresh_steps();
  }
  
  if(tick_time->tm_min % 15 == 0){
    request_weather(NULL);
  }
  
  if(tick_time->tm_min == 0) {
    HealthActivityMask activities = health_service_peek_current_activities();

    if(!(activities & HealthActivitySleep)) {
      VibePattern pat = {
        .durations = vibe_hour,
        .num_segments = ARRAY_LENGTH(vibe_hour),
      };
  
      vibes_enqueue_custom_pattern(pat);
    }
  }
}

static void handle_bluetooth(bool connected) {
  VibePattern *pat = (VibePattern*)malloc(sizeof(VibePattern));
  
  if(connected){
    if(s_bitmap_noblt_layer){
      layer_set_hidden(bitmap_layer_get_layer(s_bitmap_noblt_layer), true);
      pat->durations = vibe_connect;
      pat->num_segments = ARRAY_LENGTH(vibe_connect);
    }
  }else{
    layer_set_hidden(bitmap_layer_get_layer(s_bitmap_noblt_layer), false);
    pat->durations = vibe_disconnect;
    pat->num_segments = ARRAY_LENGTH(vibe_disconnect);
  };
    
  if(s_bitmap_noblt_layer){
    vibes_enqueue_custom_pattern(*pat);
  }
  
  //text_layer_set_text(s_connection_layer, connected ? "" : "X");
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_bgtime_layer = layer_create(bounds);
  layer_set_update_proc(s_bgtime_layer, bgtime_update_proc);  
  
  s_batt_layer = layer_create(bounds);
  layer_set_update_proc(s_batt_layer, sbatt_update_proc);  
  
  s_city_layer = text_layer_create(GRect(0, 0, bounds.size.w, 16));
  text_layer_set_text_color(s_city_layer, GColorWhite);
  text_layer_set_background_color(s_city_layer, GColorClear);
  text_layer_set_font(s_city_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentCenter);
  text_layer_set_text(s_city_layer, "Loading...");
  
  s_time_layer = text_layer_create(GRect(0, 12, bounds.size.w, 42));
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  s_date_layer = text_layer_create(GRect(0, 56, bounds.size.w, 22));
  text_layer_set_text_color(s_date_layer, GColorLightGray);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  s_sunset_layer = text_layer_create(GRect(0, 74, bounds.size.w, 16));
  text_layer_set_text_color(s_sunset_layer, GColorLightGray);
  text_layer_set_background_color(s_sunset_layer, GColorClear);
  text_layer_set_font(s_sunset_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_sunset_layer, GTextAlignmentCenter);
  text_layer_set_text(s_sunset_layer, "unavailable");
  
  s_connection_layer = text_layer_create(GRect(0, 0, bounds.size.w, 16));
  text_layer_set_text_color(s_connection_layer, GColorWhite);
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentRight);
  handle_bluetooth(connection_service_peek_pebble_app_connection());

  s_temp_layer = text_layer_create(GRect(0, bounds.size.h - 80, bounds.size.w - 50, 42));
  text_layer_set_text_color(s_temp_layer, GColorWhite);
  text_layer_set_background_color(s_temp_layer, GColorClear);
  text_layer_set_font(s_temp_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  text_layer_set_text(s_temp_layer, "0.0");
  
  s_hum_layer = text_layer_create(GRect(0, bounds.size.h - 72, bounds.size.w - 15, 16));
  text_layer_set_text_color(s_hum_layer, GColorLightGray);
  text_layer_set_background_color(s_hum_layer, GColorClear);
  text_layer_set_font(s_hum_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_hum_layer, GTextAlignmentRight);
  text_layer_set_text(s_hum_layer, "100%");
  
  s_wind_layer = text_layer_create(GRect(0, bounds.size.h - 47, bounds.size.w - 26, 16));
  text_layer_set_text_color(s_wind_layer, GColorLightGray);
  text_layer_set_background_color(s_wind_layer, GColorClear);
  text_layer_set_font(s_wind_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_wind_layer, GTextAlignmentRight);
  text_layer_set_text(s_wind_layer, "0");
  
  s_cond_layer = text_layer_create(GRect(0, bounds.size.h - 40, bounds.size.w - 50, 44));
  text_layer_set_text_color(s_cond_layer, GColorLightGray);
  text_layer_set_background_color(s_cond_layer, GColorClear);
  text_layer_set_font(s_cond_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_cond_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_cond_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_text(s_cond_layer, "Not good");
    
  s_steps_layer = text_layer_create(GRect(0, bounds.size.h - 21, bounds.size.w - 15, 16));
  text_layer_set_text_color(s_steps_layer, GColorLightGray);
  text_layer_set_background_color(s_steps_layer, GColorClear);
  text_layer_set_font(s_steps_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_steps_layer, GTextAlignmentRight);
  text_layer_set_text(s_steps_layer, "0");
  
  s_bitmap_noblt = gbitmap_create_with_resource(RESOURCE_ID_NOBLT);
  s_bitmap_noblt_layer = bitmap_layer_create(GRect(bounds.size.w - 13, 0, 13, 13));
  bitmap_layer_set_bitmap(s_bitmap_noblt_layer, s_bitmap_noblt);
  layer_set_hidden(bitmap_layer_get_layer(s_bitmap_noblt_layer), true);

  s_bitmap_sunrise = gbitmap_create_with_resource(RESOURCE_ID_SUNRISE);
  s_bitmap_sunrise_layer = bitmap_layer_create(GRect(25, 80, 13, 8));
  bitmap_layer_set_bitmap(s_bitmap_sunrise_layer, s_bitmap_sunrise);
  
  s_bitmap_sunset = gbitmap_create_with_resource(RESOURCE_ID_SUNSET);
  s_bitmap_sunset_layer = bitmap_layer_create(GRect(105, 80, 13, 8));
  bitmap_layer_set_bitmap(s_bitmap_sunset_layer, s_bitmap_sunset);
  
  s_bitmap_owmlogo = gbitmap_create_with_resource(RESOURCE_ID_OWM);
  s_bitmap_wulogo = gbitmap_create_with_resource(RESOURCE_ID_WU);
  s_bitmap_wlogo_layer = bitmap_layer_create(GRect(0, bounds.size.h - 62, 13, 20));
  bitmap_layer_set_bitmap(s_bitmap_wlogo_layer, s_bitmap_owmlogo);
  
  s_bitmap_hum = gbitmap_create_with_resource(RESOURCE_ID_HUM);
  s_bitmap_hum_layer = bitmap_layer_create(GRect(bounds.size.w - 13, bounds.size.h - 70, 13, 13));
  bitmap_layer_set_bitmap(s_bitmap_hum_layer, s_bitmap_hum);
  
  s_bitmap_wind = gbitmap_create_with_resource(RESOURCE_ID_WIND);
  s_bitmap_wind_layer = bitmap_layer_create(GRect(bounds.size.w - 13, bounds.size.h - 43, 13, 11));
  bitmap_layer_set_bitmap(s_bitmap_wind_layer, s_bitmap_wind);
  s_bitmap_windms = gbitmap_create_with_resource(RESOURCE_ID_MS);
  s_bitmap_windms_layer = bitmap_layer_create(GRect(bounds.size.w - 28, bounds.size.h - 44, 13, 13));
  bitmap_layer_set_bitmap(s_bitmap_windms_layer, s_bitmap_windms);
  
  s_bitmap_run = gbitmap_create_with_resource(RESOURCE_ID_RUN);
  s_bitmap_run_layer = bitmap_layer_create(GRect(bounds.size.w - 13, bounds.size.h - 18, 13, 12));
  bitmap_layer_set_bitmap(s_bitmap_run_layer, s_bitmap_run);
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  battery_state_service_subscribe(handle_battery);

  layer_add_child(window_layer, s_bgtime_layer);
  layer_add_child(window_layer, s_batt_layer);
  
  layer_add_child(window_layer, text_layer_get_layer(s_city_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_temp_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_cond_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_hum_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_sunset_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer)); 

  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_noblt_layer));
  
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_sunrise_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_sunset_layer));  
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_wlogo_layer));  
  
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_hum_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_wind_layer));  
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_windms_layer)); 
  
  layer_add_child(window_layer, text_layer_get_layer(s_wind_layer));
  
  #if defined(PBL_HEALTH)
  layer_add_child(window_layer, text_layer_get_layer(s_steps_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_run_layer));  

  refresh_steps();
  #else
    APP_LOG(APP_LOG_LEVEL_WARNING, "Load: Pebble Health not available.");
  #endif  
  
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = handle_bluetooth
  });
}

static void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  layer_destroy(s_bgtime_layer);
  layer_destroy(s_batt_layer);
  text_layer_destroy(s_city_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_temp_layer);
  text_layer_destroy(s_cond_layer);
  text_layer_destroy(s_hum_layer);
  text_layer_destroy(s_wind_layer);
  text_layer_destroy(s_sunset_layer);
  text_layer_destroy(s_steps_layer);
  text_layer_destroy(s_connection_layer);
  
  gbitmap_destroy(s_bitmap_noblt);
  gbitmap_destroy(s_bitmap_sunrise);
  gbitmap_destroy(s_bitmap_sunset);
  gbitmap_destroy(s_bitmap_owmlogo);
  gbitmap_destroy(s_bitmap_wulogo);
  gbitmap_destroy(s_bitmap_hum);
  gbitmap_destroy(s_bitmap_wind);
  gbitmap_destroy(s_bitmap_windms);
  gbitmap_destroy(s_bitmap_run);
  
  bitmap_layer_destroy(s_bitmap_noblt_layer);
  bitmap_layer_destroy(s_bitmap_sunset_layer);
  bitmap_layer_destroy(s_bitmap_wlogo_layer);
  
  bitmap_layer_destroy(s_bitmap_hum_layer);
  bitmap_layer_destroy(s_bitmap_wind_layer);
  bitmap_layer_destroy(s_bitmap_windms_layer);
  
  bitmap_layer_destroy(s_bitmap_run_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
  events_app_message_request_inbox_size(2026);
  events_app_message_request_outbox_size(656);
  events_app_message_register_inbox_received(inbox_received_handler, NULL);
  events_app_message_open();
  
  charge_state = (BatteryChargeState*)malloc(sizeof(BatteryChargeState));  
  *charge_state = battery_state_service_peek();
  
  loadconfig();
  
  app_timer_register(3000, request_weather, NULL);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}