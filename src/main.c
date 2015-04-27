/*
  Source originally copied from
  https://github.com/nirajsanghvi/PolarClock2.0
  
  Modifications by Tim Miller
 */

#include <main.h>
#include <time.h>
  
enum {
        KEY_TIME = 0x0,
        KEY_DATE = 0x1,
        KEY_ROW = 0x2,
        KEY_INVERT = 0x3
};

// static bool INVERT = false;

GColor BACKGROUND_COLOR; //= GColorBlack;
// static GColor FOREGROUND_COLOR; // = GColorWhite;

GColor MinHourPetalColor;
GColor SecondHandColor;
GColor MarkersColor;

Window *window;
Layer *watchface_layer;
Layer *markers_layer;

#define NUM_COLORS 64

static GColor GColors[NUM_COLORS] = {
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
static int getHemisphere(int angle) {
  int quadrant = getQuadrant(angle);
  
  if (quadrant < 3) {
    return 1;
  } else {
    return 2;
  }
}

// left half is 1, right half is 2
static int getHalf(int angle) {
  int quadrant = getQuadrant(angle);
  
  if ((quadrant == 2)||(quadrant == 3)) {
    return 1;
  } else {
    return 2;
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, click_handler);
}

static void click_handler(ClickRecognizerRef recognizer, void *context) {
  updateColors();
}

static void updateColors() {
  int randInt = rand() % NUM_COLORS;
  MinHourPetalColor = GColors[randInt];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "MinHourPetalColor index: %u", randInt);
  randInt = rand() % NUM_COLORS;
  SecondHandColor = GColors[randInt];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "SecondHandColor index: %u", randInt);
  randInt = rand() % NUM_COLORS;
  BACKGROUND_COLOR = GColors[randInt];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "BackgroundColor index: %u", randInt);
  randInt = rand() % NUM_COLORS;
  MarkersColor = GColors[randInt];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "MarkersColor index: %u", randInt);
}

static void markers_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_stroke_color(ctx, MarkersColor); //GColorDarkGray
  int angle;
  
  for(angle = 0 ; angle < 360; angle += 30) {
    gpath_rotate_to(markers_path, (TRIG_MAX_ANGLE / 360) * angle);
    gpath_draw_outline(ctx, markers_path);
  }
}

static void watchface_layer_update_callback(Layer *layer, GContext* ctx) {
  int hourOffset = 0;
  int minOffset = 0;
  
  //time_t now = time(NULL);
  //struct tm *t = localtime(&now);

  int secs = 1; //t->tm_sec;
  int mins = 19; //t->tm_min;
  int hours = 11; //t->tm_hour;
  
  unsigned int secAngle = (secs + 1) * 6;
  unsigned int minAngle = mins * 6;
  unsigned int hourAngle = (( hours % 12 ) * 30) + (mins / 2);

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, MinHourPetalColor);

  graphics_fill_circle(ctx, center, 65);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
  graphics_context_set_stroke_color(ctx, GColorClear);
  
  if (secs == 1) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "------------------------------------");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "minAngle:  %u", minAngle);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hourAngle: %u", hourAngle);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "minQuadrant:  %u", getQuadrant(minAngle));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hourQuadrant: %u", getQuadrant(hourAngle));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "minHalf:  %u", getHalf(minAngle));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hourHalf: %u", getHalf(hourAngle));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "minHemis:  %u", getHemisphere(minAngle));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hourHemis: %u", getHemisphere(hourAngle));
  }
  
  if (getHalf(minAngle) == getHalf(hourAngle)) {
    //Both on same half, either top or bottom quadrant
    //Pretty sure this is right
    if (minAngle > hourAngle) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 1");
      hourOffset = 0;
      minOffset = 180;
    } else {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 2");
      hourOffset = 180;
      minOffset = 0;
    }
  } else { 
    if (getHemisphere(minAngle) == getHemisphere(hourAngle)) {
      if (getHemisphere(minAngle) == 1) {
        //Both on top, opposite quadrants
        if (minAngle > hourAngle) {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 3");
          // 12:51pm correct with hourOffset=180 and minOffset=0. BUT WHY?
          hourOffset = 180;
          minOffset = 0;
        } else {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 4");
          // 10:00pm incorrect with hourOffset=180 and minOffset=0. Correct when flipped.
          hourOffset = 0;
          minOffset = 180;
        }
      } else {
        //Both on bottom, opposite quadrants
        if (minAngle > hourAngle) {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 5");
          // 3:31pm incorrect when hourOffset=180 and minOffset=0. Correct when it's the opposite. BUT WHY???
          hourOffset = 0;
          minOffset = 180;
        } else {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 6");
          hourOffset = 0;
          minOffset = 180;
        }
      }
    } else {
      //Caddy-corner quadrants
      // Something's wrong at 11:27pm?
      if ((minAngle-hourAngle) > 180) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 7");
        // Correct at 12:33pm (it flipped between 12:32 and 12:33 correctly!) and 1:40pm
        // INcorrect at 11:19am though! Do we need a switch on quadrant?! lol omg
        // But it's correct again as soon as it's 11:27 or so....
        hourOffset = 180;
        minOffset = 0;
      } else if ((minAngle-hourAngle) > 0) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 8");
        // Correct at 12:32pm and 1:31pm!
        hourOffset = 0;
        minOffset = 180;
      } else if (abs(minAngle-hourAngle) > 180) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 9");
        // I think switching to hourOffset=180 and minOffset=0 fixed this at 10:20
        hourOffset = 180;
        minOffset = 0;
      } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "CASE 10");
        hourOffset = 0;
        minOffset = 180;
      }
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
}

//   #if TWENTY_FOUR_HOUR_DIAL
//     angle = (t->tm_hour * 15) + (t->tm_min / 4);
//   #else
//     angle = (( t->tm_hour % 12 ) * 30) + (t->tm_min / 2);
//   #endif
// */

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(watchface_layer);
}

void in_received_handler(DictionaryIterator *received, void *context) {
//    Tuple *invert_tuple = dict_find(received, KEY_INVERT);
   
//    INVERT = invert_tuple->value->int8 ? true : false;
//    persist_write_bool(KEY_INVERT, INVERT);
//    if(INVERT)
//    {
//     BACKGROUND_COLOR = GColorWhite;
//     FOREGROUND_COLOR = GColorBlack;
//    }
//    else
//    {
//     BACKGROUND_COLOR = GColorBlack;
//     FOREGROUND_COLOR = GColorRed;        
//    }
//    window_set_background_color(window, BACKGROUND_COLOR);
 }

void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
}

static void init(void) {  
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_open(64, 0);
  
  srand( time(NULL) );
  BACKGROUND_COLOR = GColorBlack;
  updateColors();
  
  window = window_create();
  window_set_background_color(window, BACKGROUND_COLOR);
  window_stack_push(window, true);
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  window_set_click_config_provider(window, click_config_provider); 
  
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
  gpath_destroy(markers_path);
  gpath_destroy(mask_path);
  
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