/*
 *  tenkey2.h - last saved: Time-stamp: <Sat Nov 19 12:19:09 JST 2022>
 *
 *   Copyright (c) 2022  jmotohisa (Junichi Motohisa)  <motohisa@ist.hokudai.ac.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: tenkey2.h 2022-11-19 12:15:01 jmotohisa $
 */

/*
  Modified based on M5Touch.h
  /*

  /*! 
  @file tenkey2.h 
  @brief 
  @author J. Motohisa
*/

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

class EPS32Touch {
 public:
  static EPS32Touch* instance;
  EPS32Touch();
  void begin();
  uint8_t ft6336(uint8_t reg);
  void ft6336(uint8_t reg, uint8_t value);
  void ft6336(uint8_t reg, uint8_t size, uint8_t* data);
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

#endif /* _M5TOUCH_H_ */
