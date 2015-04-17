/*
  Source originally copied from
  https://github.com/nirajsanghvi/PolarClock2.0
  
  Modifications by Tim Miller
 */

#include <main.h>

enum {
        KEY_TIME = 0x0,
        KEY_DATE = 0x1,
        KEY_ROW = 0x2,
        KEY_INVERT = 0x3
};

static bool INVERT = false;

static GColor BACKGROUND_COLOR; //= GColorBlack;
static GColor FOREGROUND_COLOR; // = GColorWhite;

Window *window;
Layer *watchface_layer;

// static GPoint center = {0, 0};
// GPoint second = {0, 0};
// GPoint minute = {0, 0};
// GPoint hour = {0, 0};

const GPathInfo SECOND_SEGMENT_PATH_POINTS = {
  2,
  (GPoint []) {
    {0, 0},
    {0, -70}, // 70 = radius + fudge; 7 = 70*tan(6 degrees); 6 degrees per minute;
    // {3,  -70},
  }
};

const GPathInfo Mask = {
  4,
  (GPoint []) {
    {0, 70},
    {0, -70}, // 70 = radius + fudge; 7 = 70*tan(6 degrees); 6 degrees per minute;
    {-70,  -70},
    {-70, 70}
  }
};

static GPath *second_segment_path;
static GPath *mask_path;

// static int getQuadrant(int angle) {
//   if ((angle>0)&&(angle<=90)) {
//     return 1;
//   } else if ((angle>90)&&(angle<=180)) {
//     return 4;
//   } else if ((angle>180)&&(angle<=270)) {
//     return 3;
//   } else {
//     return 2;
//   }
// }

// // top half is 1, bottom half is 2
// static int getHemisphere(int quadrant) {
//   if (quadrant < 3) {
//     return 1;
//   } else {
//     return 2;
//   }
// }

// // left half is 1, right half is 2
// static int getHalf(int quadrant) {
//   if ((quadrant == 2)||(quadrant == 3)) {
//     return 1;
//   } else {
//     return 2;
//   }
// }

// static void drawSameQuadrantBlackout(int quadrant, int minAngle, int hourAngle) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "drawSameQuadrantBlackout");
// }

// static void drawHemisphereBlackout(int hemisphere, int minAngle, int hourAngle) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "drawHemisphereBlackout");
// }

// static void drawHalfBlackout(int half, int minAngle, int hourAngle) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "drawHalfBlackout");
// }

// static void drawBlackout(int minQuadrant, int hourQuadrant, int minAngle, int hourAngle) {
//   if (minQuadrant == hourQuadrant) {
//     // same quadrant
//     drawSameQuadrantBlackout(minQuadrant, minAngle, hourAngle);

//   } else if (getHemisphere(minQuadrant) == getHemisphere(hourQuadrant)) {
//     // both top or both bottom
//     drawHemisphereBlackout(getHemisphere(minQuadrant), minAngle, hourAngle);

//   } else if (getHalf(minQuadrant) == getHalf(hourQuadrant)) {
//     // both left or right
//     drawHalfBlackout(getHalf(minQuadrant), minAngle, hourAngle);

//   } else {
//     // caddycorner
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "caddycornerBlackout");
//   }
// }

static void watchface_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int secAngle = (t->tm_sec + 1) * 6;
  unsigned int minAngle = t->tm_min * 6;
  unsigned int hourAngle = (( t->tm_hour % 12 ) * 30); //add back in to incorporate minutes + (t->tm_min / 2);

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

  graphics_fill_circle(ctx, center, 65);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
  graphics_context_set_stroke_color(ctx, GColorClear);
  //what is this for? gpath_draw_filled(ctx, second_segment_path);

//   APP_LOG(APP_LOG_LEVEL_DEBUG, "(TRIG_MAX_ANGLE / 360) * minAngle = %u", (TRIG_MAX_ANGLE / 360) * minAngle);
  gpath_rotate_to(mask_path, (TRIG_MAX_ANGLE / 360) * minAngle);
  gpath_draw_filled(ctx, mask_path);
  gpath_rotate_to(mask_path, (TRIG_MAX_ANGLE / 360) * (hourAngle + 180));
  gpath_draw_filled(ctx, mask_path);
  
  graphics_context_set_stroke_color(ctx, GColorYellow);
  
  gpath_rotate_to(second_segment_path, (TRIG_MAX_ANGLE / 360) * secAngle);
  gpath_draw_outline(ctx, second_segment_path);
//   int minQuadrant = getQuadrant(minAngle);
//   int hourQuadrant = getQuadrant(hourAngle);

//   drawBlackout(minQuadrant, hourQuadrant, minAngle, hourAngle);

  // struct GPathInfo blackoutPath;
  // GPoint blackoutPoints [5];
  // blackoutPoints.points = blackoutPoints;

  // GPath blackout = gpath_create(&SECOND_SEGMENT_PATH_POINTS);
  // what's this for? gpath_move_to(second_segment_path, grect_center_point(&bounds));
  // gpath_destroy(blackout);

//   gpath_rotate_to(second_segment_path, (TRIG_MAX_ANGLE / 360) * minAngle);
//   gpath_draw_outline(ctx, second_segment_path);
//   gpath_rotate_to(second_segment_path, (TRIG_MAX_ANGLE / 360) * hourAngle);
//   gpath_draw_outline(ctx, second_segment_path);

}


// static void minute_display_layer_update_callback(Layer *layer, GContext* ctx) {

//   time_t now = time(NULL);
//   struct tm *t = localtime(&now);

//   unsigned int angle = t->tm_min * 6;

//   GRect bounds = layer_get_bounds(layer);
//   GPoint center = grect_center_point(&bounds);

//   graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

//   graphics_fill_circle(ctx, center, 55);

//   graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);

//   for(; angle < 355; angle += 6) {

//     gpath_rotate_to(minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

//     gpath_draw_filled(ctx, minute_segment_path);

//   }

//   graphics_fill_circle(ctx, center, 50);

// }


// static void hour_display_layer_update_callback(Layer *layer, GContext* ctx) {

//   time_t now = time(NULL);
//   struct tm *t = localtime(&now);

//   unsigned int angle;

// /*
//   #if TWENTY_FOUR_HOUR_DIAL
//     angle = (t->tm_hour * 15) + (t->tm_min / 4);
//   #else
//     angle = (( t->tm_hour % 12 ) * 30) + (t->tm_min / 2);
//   #endif
// */
  
//   angle = (( t->tm_hour % 12 ) * 30); // + (t->tm_min / 2);
    
//   angle = angle - (angle % 6);

//   GRect bounds = layer_get_bounds(layer);
//   GPoint center = grect_center_point(&bounds);

//   graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

//   graphics_fill_circle(ctx, center, 45);

//   graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);

//   for(; angle < 355; angle += 6) {

//     gpath_rotate_to(hour_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

//     gpath_draw_filled(ctx, hour_segment_path);
//   }

//   graphics_fill_circle(ctx, center, 40);
// }


static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(watchface_layer);
}

 void in_received_handler(DictionaryIterator *received, void *context) {
   Tuple *invert_tuple = dict_find(received, KEY_INVERT);
   
   INVERT = invert_tuple->value->int8 ? true : false;
   persist_write_bool(KEY_INVERT, INVERT);
   if(INVERT)
   {
    BACKGROUND_COLOR = GColorWhite;
    FOREGROUND_COLOR = GColorBlack;
   }
   else
   {
    BACKGROUND_COLOR = GColorBlack;
    FOREGROUND_COLOR = GColorRed;        
   }
   window_set_background_color(window, BACKGROUND_COLOR);
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

static void init(void) {  
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_open(64, 0);

  if(persist_exists(KEY_INVERT)) INVERT = persist_read_bool(KEY_INVERT);
  
  if(INVERT) {
    BACKGROUND_COLOR = GColorWhite;
    FOREGROUND_COLOR = GColorBlack;
  }
  else {
    BACKGROUND_COLOR = GColorBlack;
    FOREGROUND_COLOR = GColorRed;        
  }
  
  window = window_create();
  window_set_background_color(window, BACKGROUND_COLOR);
  window_stack_push(window, true);
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Init the layer for the second display
  watchface_layer = layer_create(bounds);
  layer_set_update_proc(watchface_layer, watchface_layer_update_callback);
  layer_add_child(window_layer, watchface_layer);

  // Init the second segment path
  second_segment_path = gpath_create(&SECOND_SEGMENT_PATH_POINTS);
  gpath_move_to(second_segment_path, grect_center_point(&bounds));
  
  // Init the mask path
  mask_path = gpath_create(&Mask);
  gpath_move_to(mask_path, grect_center_point(&bounds));
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
  gpath_destroy(second_segment_path);

  tick_timer_service_unsubscribe();
  window_destroy(window);
  layer_destroy(watchface_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}