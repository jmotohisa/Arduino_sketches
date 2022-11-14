// based on touchpaint example in Adafruit ILI9341 library
// Modified to work with my ESP32 Temp-logger

// Wiring: 
/*
ILI9341 module | ESPP32
-------|---------------
1 (VCC)    | 3V3
2 (GND)    | GND
3 (CS)     | 14
4 (RESET)  | 33
5 (D/C)    | 27
6 (SDI(MOSI)) | 23
7 (SCK)    | 18
8 (LED)    | 3V3 (with 100ohm)
9 (SDO(MISO) | 19
10 (T_CLK) | 18 SCLK
11 (T_CS)  | 5
12 (T_DIN) | 23 MOSI
13 (T_DO)  | 19 MISO
14 (T_IRQ) | NC(4)
 */

/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341 Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
//#include "myXPT2046_Touchscreen.h"

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 3800
#define TS_MAXX 500
#define TS_MINY 500
#define TS_MAXY 3700
/*
 *       cfg.x_min      = 3700;    // タッチスクリーンから得られる最小のX値(生の値)
      cfg.x_max      = 360;  // タッチスクリーンから得られる最大のX値(生の値)
      cfg.y_min      = 430;    // タッチスクリーンから得られる最小のY値(生の値)
      cfg.y_max      = 3700;  // タッチスクリーンから得られる最大のY値(生の値)

 */
// CS of touchscreen
#define TOUCH_CS 5
#define TOUCH_IRQ 4
//XPT2046_Touchscreen ts = XPT2046_Touchscreen(TOUCH_CS);
XPT2046_Touchscreen ts = XPT2046_Touchscreen(TOUCH_CS,TOUCH_IRQ);


// The display also uses hardware SPI
#define TFT_CS 14
#define TFT_RST 33
#define TFT_DC 27
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_MISO 19
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC,TFT_RST);

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void) {
 // while (!Serial);     // used for leonardo debugging
 
  Serial.begin(115200);
  Serial.println(F("Touch Paint!"));
  
  tft.begin();

  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  
  tft.fillScreen(ILI9341_BLACK);
  
  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
 
  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
}


void loop()
{
  // See if there's any  touch data for us
  if (ts.bufferEmpty()) {
    return;
  }
  /*
  // You can also wait for a touch
  if (! ts.touched()) {
    return;
  }
  */

  // Retrieve a point
  if(ts.tirqTouched()) {
  TS_Point p0 = ts.getPoint();
  if(p0.x>100 & p0.y>100) {
  
 /*
  Serial.print("X = "); Serial.print(p.x);
  Serial.print("\tY = "); Serial.print(p.y);
  Serial.print("\tPressure = "); Serial.println(p.z);  
 */
 
  // Scale from ~0->4000 to tft.width using the calibration #'s
  // and swap x and y
  TS_Point p;
  p.y = map(p0.x, TS_MINX, TS_MAXX, 0, tft.height());
  p.x = map(p0.y, TS_MINY, TS_MAXY, 0, tft.width());
  Serial.print("x=");
  Serial.print(p0.x);
  Serial.print(", y=");
  Serial.print(p0.y);
  Serial.print("; Mapped: x=");
  Serial.print(p.x);
  Serial.print(", y=");
  Serial.println(p.y);

  /*
  Serial.print("("); Serial.print(p.x);
  Serial.print(", "); Serial.print(p.y);
  Serial.println(")");
  */

  if (p.y < BOXSIZE) {
     oldcolor = currentcolor;

     if (p.x < BOXSIZE) { 
       currentcolor = ILI9341_RED; 
       tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (p.x < BOXSIZE*2) {
       currentcolor = ILI9341_YELLOW;
       tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (p.x < BOXSIZE*3) {
       currentcolor = ILI9341_GREEN;
       tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (p.x < BOXSIZE*4) {
       currentcolor = ILI9341_CYAN;
       tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (p.x < BOXSIZE*5) {
       currentcolor = ILI9341_BLUE;
       tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (p.x < BOXSIZE*6) {
       currentcolor = ILI9341_MAGENTA;
       tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     }

     if (oldcolor != currentcolor) {
        if (oldcolor == ILI9341_RED) 
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
        if (oldcolor == ILI9341_YELLOW) 
          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
        if (oldcolor == ILI9341_GREEN) 
          tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
        if (oldcolor == ILI9341_CYAN) 
          tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
        if (oldcolor == ILI9341_BLUE) 
          tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
        if (oldcolor == ILI9341_MAGENTA) 
          tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
     }
  }
  if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
    tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
  }
  }
  }
}
