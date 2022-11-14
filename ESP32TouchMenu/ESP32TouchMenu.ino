// taken from : https://www.hackster.io/nash-ali/using-the-ili9341-tft-xpt2046-touch-with-esp32-arduino-ac8eed

/*
  Created:      11:06 PM 9/15/2020
  Last Updated: 4:45 PM 2/21/2021
  MIT License

  Copyright (c) 2021 Zulfikar Naushad Ali

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.


  Uses the NodeMCU Devkit esp32 wroom 32 module (38pin)
  In this application the need for exact pixel-to-touch positioning is not required.
  Hence the lack of calibration software. I use the technique "ball-park" method to
  derive co-ord info. It's a little rough mind you.


  I2C Device Listing
  0x33 - MLX90640 Flir
  0x3C - OLED - not used
  0x3D - OLED - not used
  0x40 - HDC1080 Humidity
  0x57 - AT24C32 address
  0x5A - CCS811 AQ
  0x5B - MLX90614 (note: needs to have its address changed)done! OK 9-15-20 ZNA
  0x60 - Si1145 LQ
  0x68 - RTC - DS3231
  0x76 - BME280

  SPI Listing
  ILI9341 Display Controller
  display - Click here - https://www.amazon.ca/gp/product/B087C3PP9G/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1
  ThingPulse_XPT2046 Touch Controller
  see - https://github.com/PaulStoffregen/XPT2046_Touchscreen

  Latest Modifications: 2/15/21

  Create Helper.h file
  Add Button command engine (basic)
  function that takes x,y and returns 0 if out of range, 1 - n, n=number of buttons available

*/

#include <WiFi.h>                 //  Wifi
#include <WiFiUdp.h>              //  UDP
#include <NTPClient.h>            //  time server
#include <SD.h>                   //  SD Card
//#include <TinyGsmClient.h>      //  cell stuff.
#include <SPI.h>                  //  spi
#include <Adafruit_GFX.h>         //  graphics
#include <Adafruit_ILI9341.h>     //  display
#include <Adafruit_MLX90614.h>    //  Melexis IR temp
#include <Adafruit_MLX90640.h>    //  Flir
#include <MAX30105.h>             //  MAX30102 oxi/pulse (on hold just yet)new compile to ESP32 WROOM 32 today (9-15-2020)
#include <heartRate.h>            //  Heart rate calculating algorithm
#include <XPT2046_Touchscreen.h>  //  touch
#include <SparkFunBME280.h>       //  Click here to get the library: http://librarymanager/All#SparkFun_BME280
#include <SparkFunCCS811.h>       //  Air Quality Index
#include <ClosedCube_HDC1080.h>   //  temp & humidity
#include <Adafruit_SI1145.h>      //  GY-1145 light quality sensor
#include <RTClib.h>               //  RTC library
#include <time.h>                 //  standard time stuff
#include <Adafruit_MPU6050.h>     //  Gyro
#include <Adafruit_TCS34725.h>    //  RGB Sensor

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//***********************************************************************
#include "Helper.h"               //  my helper header
//  SETUP ***************************************************************
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);// for debug purposes.
  // Keep power when running from battery
  //bool isOk = setPowerBoostKeepOn(1);
  InitDisplay();
  showBiosInfo();
  scanI2CBus();
  initializeSensorGroup();
  delay(2000);
  ClearScreen();
  infoBlocks();
  MENU_MODE = 0;
  showMenuSystem(MENU_MODE);
  delay(200);
  initMyTouch();
  delay(200);
  clearStatus();
  //BUILTIN LED
  pinMode (2, OUTPUT);
  digitalWrite(2, LOW);
  //touch irq pin
  pinMode (35, INPUT);
  AttachTouch();
}
//  LOOP  *******************************************************************
void loop() {
  //set the BUILTIN_LED off.
  digitalWrite(BUILTIN_LED, LOW);
  // if TouchType is 0 then here is where the actual parsing of the button occurs  - action();
  // first we check for keypress by looking at PAD_DOWN event
  //action(); will invoke the loop
  if (PAD_DOWN && GLOBAL_TOUCH_TYPE == MENU) {
    // at this point to remember is that the interrupt routine
    //just returns the vales to TouchX & TouchY and sets PAD_DOWN = true
    // here we can take the time to analyze x,y and come up with BUTTON_NUMBER
    BUTTON_NUMBER = parseMenuTouch(TouchX , TouchY );
    action(BUTTON_NUMBER);
    PAD_DOWN = false;
  }
  if (PAD_DOWN && GLOBAL_TOUCH_TYPE == NUMERIC) {

    // has a valid entry(number,.,<OK>) been entered?
    GLOBAL_NUMBER = parseForNumber(TouchX, TouchY);
    PAD_DOWN = false;
  }
  // next skip through the loop...anything else you need to loop
  //delay(250);//debounce for now...
  // all of the items you need to run can be done like this...
  // just activate your items one at a time set SystemMode to  1-4.
  if (SystemMode == 0) {
    //display what you need to in your idle loop

  }
  if (SystemMode == TIME) {
    showDateTime();
  }
  if (SystemMode == GYRO) {
    showGyroData();
  }
  if (SystemMode == OXIMETER) {
    //display what you need to in your oxi loop
  }
  if (SystemMode == FLIR) {
    //display what you need to in your FLIR loop
    updateFlirImage();
  }
  if (SystemMode == ENVIRONMENT) {
    //display what you need to in your BME280 loop
    showWeather();
  }
  if (SystemMode == RGB_SCAN) {
    //display what you need to in your BME280 loop
    showRGBReader();
  }
  // and so on...
}// end of loop
