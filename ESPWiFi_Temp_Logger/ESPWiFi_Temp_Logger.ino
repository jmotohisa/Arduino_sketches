/*
  ESPWiFi_Temp_Logger.ino
*/

/*
 * Hardware
 *  ESP32 DevKit
 *  MAX6675 thermocouple module
 *  ILI9341 LCD module (with SD and XPT2046 Touchscreen)
 *  DS1307 RTC module (with I2C level converter)
*/
  
/*
  MAX31855/MAX6675 熱電対センサの値を10分ごとにSDカードに記録する
  記録したデータはHTTPを立てて外部からブラウザ経由で見れる

  based on Swich Science ESPWiFI_SD_Logger
  http://mag.switch-science.com/

  SDcardディレクトリの中身をESP-WROOM-02に接続するSDカードのルートディレクトリにコピーしてください
*/

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
//#include <Adafruit_MAX31855.h>
#include "JM_MAX6675.h"
#include <XPT2046_Touchscreen.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
//#include "BluetoothSerial.h"

//BluetoothSerial SerialBT;

#include <Time.h>
#include <TimeLib.h>

#include "NTP.h"
#include "SDlogger.h"
#include "WebServer.h"

#define UTC_TOKYO +9

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* pass = STAPSK;

// Wiring
/*
EPS32 | MAX31885 peripheral
--------|---------
18      | 4 (SCK)
19      | 3 (MISO)
16      | 1 (SS)
GND     | 5 (GND)
3V3     | 6 (VCC)

ESP32 | MAX 6675
------|------
19 | MISO
16 | SS
18 | SCK
3V3 | VCC
GND | GND

ILI9341 module | EPS32
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
14 (T_IRQ) | NC

R1 (CS)     | 17
R2 (MOSI)   | MOSI
R3 (MISO)   | MISO
R4(J4) (SCK)    | SCK

RTC DS1307 (via level converter) | ESP32
SCL | 22
SDA | 21


 */
#define TFT_CS 14
#define TFT_RST 33
#define TFT_DC 27
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_MISO 19

#define TOUCH_SCLK TFT_CLK
#define TOUCH_CS 5
#define TOUCH_DIN TFT_MOSI
#define TOUCH_DOUT TFT_MISO
#define TOUCH_IRQ 4

#define MAXCLK  TFT_CLK
#define MAXDO   TFT_MISO
#define MAXCS   16

#define SD_CS 17
#define SD_MOSI TFT_MOSI
#define SD_MISO TFT_MISO
#define SD_SCK TFT_CLK

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC,TFT_RST);

XPT2046_Touchscreen ts = XPT2046_Touchscreen(TOUCH_CS);

// initialize the Thermocouple
//Adafruit_MAX31855 thermocouple(MAXCS);
JM_MAX6675 thermocouple(MAXCS);


RTC_DS1307 RTC;

int xmin,xmax,ymin,ymax;
int i=0;
double Tmin=10.0,Tmax=30.0;
boolean wastouched = true;

unsigned long whenCountStarted = 0;

//double temp,press,hum;

char date_ym[8]; // yyyy-nn0
char date_mdhm[13];  // mm/dd-hh:mmn0

//const int period = 10*60*1000; // ログを取る間隔:10分間 (ms)
const int period = 10*1000;
int ntpAccsessTimes = 0;

unsigned long unixtime;

NTPClient ntp;
SDlogger sd;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ntp.udpSetup();
  WifiServerinit();

  if(!RTC.begin())
    Serial.println("RTC not working. Skipping...");
  else
    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running! Reset date");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    RTC.adjust(ntp.getTime(&unixtime));
  }
  
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");
  
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
//  touch.begin(ucg.getWidth(), ucg.getHeight())
  ts.begin();
  tft.setRotation(1);
  ts.setRotation(1);
  
  setSDStatus(sd.init());

  int w=tft.width(),h=tft.height();
  xmin=30;
  xmax=w;
  ymin=h-20;
  ymax=50;
  tft.drawFastHLine(xmin-10,ymin,xmax-xmin+10,ILI9341_RED);
  tft.drawFastHLine(xmin-10,ymax,xmax-xmin+10,ILI9341_RED);

  tft.drawFastVLine(xmin,ymax-10,ymin-ymax+20,ILI9341_WHITE);
  tft.setCursor(0,17*3);tft.print((int) Tmax);
  tft.setCursor(0,17*13);tft.print((int) Tmin);

//  SerialBT.begin("ESP32test");
}

void loop()
{
  double c;
  DateTime now = RTC.now(); 
  
  handleClient();
  // 約50日ごとにおこるオーバーフロー時はmillsが0になるので引き算の結果はマイナス
  // とりうる値の範囲を考慮して割り算した結果同士を比較 (4,294,967,295)
  if((signed long)(millis()/1000000 - whenCountStarted/1000000) < 0){whenCountStarted = millis();}
  if((millis() - whenCountStarted) > period)
  {
    whenCountStarted = millis();

    // 時間を取得
    if((ntpAccsessTimes==0) && (ntp.getTime(&unixtime)))
    {
      ntpAccsessTimes++;
      if(ntpAccsessTimes >= 480){ ntpAccsessTimes = 0;} // 48回に1回のアクセスとする: 8時間に1回
    }else{
      //時刻取得に失敗 or 取得タイミングでなかったら前回の値から測定間隔分だけ秒数を足す
      unixtime += period/1000;
    }
    time_t t = unixtime + (UTC_TOKYO * 60 * 60); // 日本標準時に調整
    sprintf(date_ym, "%04d-%02d", year(t), month(t));
    sprintf(date_mdhm, "%2d/%02d-%02d:%02d", month(t), day(t),hour(t), minute(t));
    Serial.println(date_ym);
    Serial.println(date_mdhm);

    // センサからデータを取得

  double c1=thermocouple.readInternal();
  Serial.print("Internal Temp = ");
  Serial.println(c1);
//  SerialBT.println(c1);
  tft.setTextColor(ILI9341_WHITE);tft.setTextSize(2);
  tft.setCursor(0,0);
  tft.print("Internal Temp = ");
  tft.setCursor(12*17,0);tft.print("     ");
  tft.setCursor(12*17,0);tft.print(c1);

   c = thermocouple.readCelsius();

   tft.setCursor(0,17);tft.print("                            ");
   tft.setCursor(0,17);
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
     tft.println("Something wrong with thermocouple!");     
   } else {
     Serial.print("C = ");
     Serial.println(c);
     tft.print("C =      ");
     tft.setCursor(12*4,17);
     tft.print(c);
   }

   int x=i%(xmax-xmin)+xmin;
   int y=(c-Tmin)/(Tmax-Tmin)*(ymax-ymin)+ymin;
   tft.fillCircle(x,y,3,ILI9341_CYAN);
   i+=5;
   if(i>xmax)
    i=0;
    
    // SDカードにデータを記録
    // データを記録するファイル名の指定
    // ファイル名8文字+拡張子3文字までなので注意
    String fileName = "/";
    fileName += String(date_ym);
    fileName +=  ".csv";
    Serial.println(fileName);
    // 記録するデータの生成
    String dataStream = "";
    dataStream += String(date_mdhm);
    dataStream += String(',');
    dataStream += String(c);
    dataStream += String(',');
    dataStream += String(c1);
    dataStream += String(',');
    dataStream += String('0');
    sd.recordData(fileName,dataStream);
  }

  if (ts.touched()) {
    TS_Point p = ts.getPoint();
 //   uint16_t x, y;
 //   ts.getPosition(x, y);
/*
    if (prev_x == 0xffff) {
      ucg.drawPixel(x, y);
    } else {
      ucg.drawLine(prev_x, prev_y, x, y);
    }
    prev_x = x;
    prev_y = y;
  } else {
    prev_x = prev_y = 0xffff;
  }
  */
    {
      whenCountStarted = millis();
      
      // 時間を取得
      if((ntpAccsessTimes==0) && (ntp.getTime(&unixtime)))
	{
	  ntpAccsessTimes++;
	  if(ntpAccsessTimes >= 480){ ntpAccsessTimes = 0;} // 48回に1回のアクセスとする: 8時間に1回
	}else{
	//時刻取得に失敗 or 取得タイミングでなかったら前回の値から測定間隔分だけ秒数を足す
	unixtime += period/1000;
      }
      time_t t = unixtime + (UTC_TOKYO * 60 * 60); // 日本標準時に調整
      sprintf(date_ym, "%04d-%02d", year(t), month(t));
      sprintf(date_mdhm, "%2d/%02d-%02d:%02d", month(t), day(t),hour(t), minute(t));
      Serial.println(date_ym);
      Serial.println(date_mdhm);
      
      // センサからデータを取得
      tft.setTextColor(ILI9341_WHITE);tft.setTextSize(2);
      /*
	double c1=thermocouple.readInternal();
	Serial.print("Internal Temp = ");
	Serial.println(c1);
	//  SerialBT.println(c1);
	tft.setCursor(0,0);
	tft.print("Internal Temp = ");
	tft.setCursor(12*17,0);tft.print("     ");
	tft.setCursor(12*17,0);tft.print(c);
      */
      c = thermocouple.readCelsius();
      
      tft.setCursor(0,17);tft.print("                            ");
      tft.setCursor(0,17);
      if (isnan(c)) {
	Serial.println("Something wrong with thermocouple!");
	tft.println("Something wrong with thermocouple!");     
      } else {
	Serial.print("C = ");
	Serial.println(c);
	tft.print("C =      ");
	tft.setCursor(12*4,17);
	tft.print(c);
      }
      
      int x=i%(xmax-xmin)+xmin;
      int y=(c-Tmin)/(Tmax-Tmin)*(ymax-ymin)+ymin;
      tft.fillCircle(x,y,3,ILI9341_CYAN);
      i+=5;
      if(i>xmax)
	i=0;
      
      // SDカードにデータを記録
      // データを記録するファイル名の指定
      // ファイル名8文字+拡張子3文字までなので注意
      String fileName = "/";
      fileName += String(date_ym);
      fileName +=  ".csv";
      Serial.println(fileName);
      // 記録するデータの生成
      String dataStream = "";
      dataStream += String(date_mdhm);
      dataStream += String(',');
      dataStream += String(c);
      dataStream += String(',');
      dataStream += String(c);
      dataStream += String(',');
      dataStream += String('0');
      sd.recordData(fileName,dataStream);
    }
  
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    //   uint16_t x, y;
    //   ts.getPosition(x, y);
    /*
      if (prev_x == 0xffff) {
      ucg.drawPixel(x, y);
      } else {
      ucg.drawLine(prev_x, prev_y, x, y);
      }
      prev_x = x;
      prev_y = y;
      } else {
      prev_x = prev_y = 0xffff;
      }
    */
    Serial.print("x=");
    Serial.println(p.x);
    Serial.print("y=");
    Serial.println(p.y);
  }
  
}

void checkFile(char *fileName)
{
  File dataFile = SD.open(fileName, FILE_WRITE);
  if(!dataFile) {
    SD.open(fileName);
  } 
}
