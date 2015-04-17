#include <pebble.h>

static int getQuadrant(int angle);
// static int getHemisphere(int quadrant);
static int getHalf(int quadrant);

// static void drawBlackout(int minQuadrant, int hourQuadrant, int minAngle, int hourAngle);
// static void drawSameQuadrantBlackout(int quadrant, int minAngle, int hourAngle);
// static void drawHemisphereBlackout(int hemisphere, int minAngle, int hourAngle);
// static void drawHalfBlackout(int half, int minAngle, int hourAngle);

static void watchface_layer_update_callback(Layer *layer, GContext* ctx);
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed);
void in_received_handler(DictionaryIterator *received, void *context);

static void init(void);

static void deinit(void);
int main(void);