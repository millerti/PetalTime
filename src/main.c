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

static GColor MinHourPetalColor;
static GColor SecondHandColor;

Window *window;
Layer *watchface_layer;
Layer *markers_layer;

static GColor GColors[64] = {
        (GColor) GColorBlackARGB8, 
        (GColor) GColorOxfordBlueARGB8, 
        (GColor) GColorDukeBlueARGB8, 
        (GColor) GColorBlueARGB8, 
        (GColor) GColorDarkGreenARGB8, 
        (GColor) GColorMidnightGreenARGB8, 
        (GColor) GColorCobaltBlueARGB8, 
        (GColor) GColorBlueMoonARGB8, 
        (GColor) GColorIslamicGreenARGB8, 
        (GColor) GColorJaegerGreenARGB8, 
        (GColor) GColorTiffanyBlueARGB8, 
        (GColor) GColorVividCeruleanARGB8, 
        (GColor) GColorGreenARGB8, 
        (GColor) GColorMalachiteARGB8, 
        (GColor) GColorMediumSpringGreenARGB8, 
        (GColor) GColorCyanARGB8, 
        (GColor) GColorBulgarianRoseARGB8, 
        (GColor) GColorImperialPurpleARGB8, 
        (GColor) GColorIndigoARGB8, 
        (GColor) GColorElectricUltramarineARGB8, 
        (GColor) GColorArmyGreenARGB8, 
        (GColor) GColorDarkGrayARGB8, 
        (GColor) GColorLibertyARGB8, 
        (GColor) GColorVeryLightBlueARGB8, 
        (GColor) GColorKellyGreenARGB8, 
        (GColor) GColorMayGreenARGB8, 
        (GColor) GColorCadetBlueARGB8, 
        (GColor) GColorPictonBlueARGB8, 
        (GColor) GColorBrightGreenARGB8, 
        (GColor) GColorScreaminGreenARGB8, 
        (GColor) GColorMediumAquamarineARGB8, 
        (GColor) GColorElectricBlueARGB8, 
        (GColor) GColorDarkCandyAppleRedARGB8, 
        (GColor) GColorJazzberryJamARGB8, 
        (GColor) GColorPurpleARGB8, 
        (GColor) GColorVividVioletARGB8, 
        (GColor) GColorWindsorTanARGB8, 
        (GColor) GColorRoseValeARGB8, 
        (GColor) GColorPurpureusARGB8, 
        (GColor) GColorLavenderIndigoARGB8, 
        (GColor) GColorLimerickARGB8, 
        (GColor) GColorBrassARGB8, 
        (GColor) GColorLightGrayARGB8, 
        (GColor) GColorBabyBlueEyesARGB8, 
        (GColor) GColorSpringBudARGB8, 
        (GColor) GColorInchwormARGB8, 
        (GColor) GColorMintGreenARGB8, 
        (GColor) GColorCelesteARGB8, 
        (GColor) GColorRedARGB8, 
        (GColor) GColorFollyARGB8, 
        (GColor) GColorFashionMagentaARGB8, 
        (GColor) GColorMagentaARGB8, 
        (GColor) GColorOrangeARGB8, 
        (GColor) GColorSunsetOrangeARGB8, 
        (GColor) GColorBrilliantRoseARGB8, 
        (GColor) GColorShockingPinkARGB8, 
        (GColor) GColorChromeYellowARGB8, 
        (GColor) GColorRajahARGB8, 
        (GColor) GColorMelonARGB8, 
        (GColor) GColorRichBrilliantLavenderARGB8, 
        (GColor) GColorYellowARGB8, 
        (GColor) GColorIcterineARGB8, 
        (GColor) GColorPastelYellowARGB8, 
        (GColor) GColorWhiteARGB8
};

const GPathInfo SECOND_SEGMENT_PATH_POINTS = {
  2,
  (GPoint []) {
    {0, 0},
    {0, -70} // 70 = radius + fudge; 7 = 70*tan(6 degrees); 6 degrees per minute;
    // {3,  -70},
  }
};

const GPathInfo Mask = {
  4,
  (GPoint []) {
    {0, 69},
    {0, -69},
    {-69,  -69},
    {-69, 69}
  }
};

const GPathInfo Markers = {
  2,
  (GPoint []) {
    {0, 66},
    {0, 70}
  }
};

static GPath *second_segment_path;
static GPath *mask_path;
static GPath *markers_path;

static int getQuadrant(int angle) {
  if (angle<90) {
    return 1;
  } else if (angle<180) {
    return 4;
  } else if (angle<270) {
    return 3;
  } else {
    return 2;
  }
}

// // top half is 1, bottom half is 2
// static int getHemisphere(int quadrant) {
//   if (quadrant < 3) {
//     return 1;
//   } else {
//     return 2;
//   }
// }

// left half is 1, right half is 2
static int getHalf(int angle) {
  int quadrant = getQuadrant(angle);
  
  if ((quadrant == 2)||(quadrant == 3)) {
    return 1;
  } else {
    return 2;
  }
}

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


static void markers_layer_update_callback(Layer *layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  int angle;
  
  for(angle = 0 ; angle < 360; angle += 30) {

    gpath_rotate_to(markers_path, (TRIG_MAX_ANGLE / 360) * angle);
    gpath_draw_outline(ctx, markers_path);
  }
}

static void watchface_layer_update_callback(Layer *layer, GContext* ctx) {
  int hourOffset = 0;
  int minOffset = 0;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int secAngle = (t->tm_sec + 1) * 6;
  unsigned int minAngle = t->tm_min * 6;
  unsigned int hourAngle = (( t->tm_hour % 12 ) * 30) + (t->tm_min / 2);

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, MinHourPetalColor);

  graphics_fill_circle(ctx, center, 65);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
  graphics_context_set_stroke_color(ctx, GColorClear);

  // Incorrect at 2:33pm; wrong until 2:40pm.
  // 5:25pm virtually invisible red (correct at 5:24 and 5:26)
  // Incorrect at 6:31pm; angle on opposite side. Fixed by removing = in getQuadrant!
  //   I can't believe that that change didn't introduce new bugs at other times, though....
  // Incorrect at about 8:10pm... slowly noticed that the wrong sides were moving.
  // Incorrect at 5:32pm
  // Incorrect at 8:29 PM, correct again at 8:30pm (with precise hour hand so bear that in mind)
  
  if (getHalf(minAngle) == getHalf(hourAngle)) {
    if (minAngle > hourAngle) {
      hourOffset = 0;
      minOffset = 180;
    } else {
      hourOffset = 180;
      minOffset = 0;
    }
  } else { 
    if (minAngle > hourAngle) {
      hourOffset = 180;
      minOffset = 0;
    } else {
      hourOffset = 0;
      minOffset = 180;
    }
  }

//   APP_LOG(APP_LOG_LEVEL_DEBUG, "(TRIG_MAX_ANGLE / 360) * minAngle = %u", (TRIG_MAX_ANGLE / 360) * minAngle);
  gpath_rotate_to(mask_path, (TRIG_MAX_ANGLE / 360) * (minAngle + minOffset));
  gpath_draw_filled(ctx, mask_path);
  gpath_rotate_to(mask_path, (TRIG_MAX_ANGLE / 360) * (hourAngle + hourOffset));
  gpath_draw_filled(ctx, mask_path);
  
  graphics_context_set_stroke_color(ctx, SecondHandColor);
  
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
  
  srand(time(NULL));
  int randInt = rand() * 64;
  MinHourPetalColor = GColors[randInt];
  randInt = rand() * 64;
  SecondHandColor = GColors[randInt];
  BACKGROUND_COLOR = GColorBlack;
  
  if(persist_exists(KEY_INVERT)) INVERT = persist_read_bool(KEY_INVERT);
  
//   if(INVERT) {
//     BACKGROUND_COLOR = GColorWhite;
//     FOREGROUND_COLOR = GColorBlack;
//   }
//   else {
//     BACKGROUND_COLOR = GColorBlack;
//     FOREGROUND_COLOR = GColorRed;        
//   }
  
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
  
  // Init the markers path
  markers_path = gpath_create(&Markers);
  gpath_move_to(markers_path, grect_center_point(&bounds));
  
  // Init the layer for the markers display
  markers_layer = layer_create(bounds);
  layer_set_update_proc(markers_layer, markers_layer_update_callback);
  layer_add_child(window_layer, markers_layer);  
  
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
  gpath_destroy(second_segment_path);

  tick_timer_service_unsubscribe();
  window_destroy(window);
  layer_destroy(watchface_layer);
  layer_destroy(markers_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}