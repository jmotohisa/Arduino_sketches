// TouchRegion

#ifndef _M5TOUCH_H_
#define _M5TOUCH_H_

#include <M5Display.h>

#include "utility/Config.h"  // Defines 'TFT', a pointer to the display
#include "utility/PointAndZone.h"

#define TOUCH_W 320
#define TOUCH_H 280
#define CST_DEVICE_ADDR 0x38
#define CST_INT 39

// Strangely, the value 13 leads to slightly more frequent updates than 10
// (Still not every 13 ms, often more like 15 to 20)
#define DEFAULT_INTERVAL 13

class TouchRegion {
 public:
  static M5Touch* instance;
  M5Touch();
  void begin();
  uint8_t interval(uint8_t ivl);
  uint8_t interval();
  void update();
  bool read();
  bool ispressed();
  void dump();
  Point getPressPoint();
  uint8_t points;
  bool changed, wasRead;
  Point point[2];
  uint8_t point0finger;

 protected:
  uint8_t _interval;
  uint32_t _lastRead;
};
