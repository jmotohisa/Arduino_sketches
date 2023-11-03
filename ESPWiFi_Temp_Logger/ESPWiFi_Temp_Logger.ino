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

#define USE_BT_SERIAL
// #define ENABLE_WEBSERVER

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
//#include <Adafruit_MAX31855.h>
#include <JM_MAX6675.h>
#include <XPT2046_Touchscreen.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <Time.h>
#include <TimeLib.h>

#include "NTP.h"
#include "SDlogger.h"

#ifdef USE_BT_SERIAL
  #include "BluetoothSerial.h"
  BluetoothSerial SerialBT;
#endif

#ifdef ENABLE_WEBSERVER
  #include "WebServer.h"
#endif

#define UTC_TOKYO +9
#include "mySSID.h"
const char* ssid = STASSID;
const char* pass = STAPSK;
#define WiFiAttempts 10


// Wiring
/*
ESP32 | MAX31885 peripheral
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

XPT2046_Touchscreen ts= XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);  // Param 2 - Touch IRQ Pin - interrupt enabled polling

// initialize the Thermocouple
//Adafruit_MAX31855 thermocouple(MAXCS);
JM_MAX6675 thermocouple(MAXCS);

// Real time clock
RTC_DS1307 RTC;

NTPClient ntp;
SDlogger sd;

int xmin,xmax,ymin,ymax;
int i=0;
double Tmin=10.0,Tmax=30.0;
int interval=10;

// globals
unsigned long whenCountStarted = 0;

char date_ym[8]; // yyyy-nn0
char date_mdhm[13];  // mm/dd-hh:mmn0

//const int period = 10*60*1000; // ログを取る間隔:10分間 (ms)
const int period = interval*1000;
int ntpAccsessTimes = 0;
boolean WiFi_valid = false;

unsigned long unixtime;

typedef struct BUTTON{
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t t;
  char *key;
  uint8_t status;
} BUTTON;

#define LOOP 0
#define WAIT 1
#define SETTING 1
BUTTON key0_0 = { 195, 220,45,100,"setting",WAIT};
BUTTON *key0_array[1] = { &key0_0 };

#define KEYIN  0
#define KEYCLR 1
#define KINFIN 2
#define SKIP   3
BUTTON key1_7 = { 20, 20,45,45,"7",KEYIN};
BUTTON key1_8 = { 70, 20,45,45,"8",KEYIN};
BUTTON key1_9 = {120, 20,45,45,"9",KEYIN};
BUTTON key1_4 = { 20, 70,45,45,"4",KEYIN};
BUTTON key1_5 = { 70, 70,45,45,"5",KEYIN};
BUTTON key1_6 = {120, 70,45,45,"6",KEYIN};
BUTTON key1_1 = { 20,120,45,45,"1",KEYIN};
BUTTON key1_2 = { 70,120,45,45,"2",KEYIN};
BUTTON key1_3 = {120,120,45,45,"3",KEYIN};
BUTTON key1_0 = { 20,170,45,45,"0",KEYIN};
BUTTON key1_clr = { 70,170,45,45,"CL",KEYCLR};
BUTTON key1_ok1 = {120,170,45,45,"OK",KINFIN};

BUTTON *key1_array[12] = { &key1_0, &key1_1, &key1_2, &key1_3, &key1_4, &key1_5, &key1_6, &key1_7,
			   &key1_8, &key1_9, &key1_clr, &key1_ok1};


#define INTERVAL  0
#define SETTMIN 1
#define SETTMAX 2
#define EXIT   3
BUTTON key2_0 = { 20, 20,45,45,"time",INTERVAL};
BUTTON key2_1 = { 70, 20,45,45,"Tmin",SETTMIN};
BUTTON key2_2 = {120, 20,45,45,"Tmax",KEYIN};
BUTTON key2_3 = { 20, 70,45,45,"exit",EXIT};
BUTTON *key2_array[4] = { &key2_0, &key2_1, &key2_2, &key2_3};

#define CALIBRATION_POINT 20

float calibration_x1;
float calibration_y1;
float delta_x,delta_y;
#define MAXLEN 256
char strbuf[MAXLEN];
boolean touch_pressed;

void draw_button(BUTTON *button, uint16_t color) {
  tft.drawRect(button->x,button->y,button->w,button->t,color);
  tft.setCursor(button->x+15, button->y+12);
  tft.setTextColor(color);
  char s[3];
  strcpy(s,button->key);
  tft.setTextSize(2);
  tft.print(s);
}

void draw_settingButton() {
  int i;
  for(i=0;i<1;i++) {
    draw_button(key0_array[i],ILI9341_YELLOW);
  }
}

void draw_keys1() {
  int i;
  for(i=0;i<12;i++) {
    draw_button(key1_array[i],ILI9341_WHITE);
  }
}

void draw_keys2() {
  int i;
  for(i=0;i<4;i++) {
    draw_button(key2_array[i],ILI9341_WHITE);
  }
}

boolean is_in_area(BUTTON *button, TS_Point point) {
  return((convert_x(point.x) > button->x) && ((convert_x(point.x) <button->x +button->w))
	 &&(convert_y(point.y) > button->y) && ((convert_y(point.y) <button->y +button->t)));
}

void drawAxis()
{
  int w=tft.width(),h=tft.height();
  xmin=30;
  xmax=w;
  ymin=h-20;
  ymax=50;
  tft.drawFastHLine(xmin-10,ymin,xmax-xmin+10,ILI9341_RED);
  tft.drawFastHLine(xmin-10,ymax,xmax-xmin+10,ILI9341_RED);

  tft.drawFastVLine(xmin,ymax-10,ymin-ymax+20,ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0,xmin);tft.print((int) Tmax);
  tft.setCursor(0,xmax);tft.print((int) Tmin);
}

float convert_x(float x0){
  return (CALIBRATION_POINT+(float)(x0-calibration_x1)*delta_x);
}

float convert_y(float y0){
  return (CALIBRATION_POINT+(float)(y0-calibration_y1)*delta_y);
}

void printpoint_raw(TS_Point p){
    Serial.print("Pressure = ");
    Serial.print(p.z);
    Serial.print(", x = ");
    Serial.print(p.x);
    Serial.print(", y = ");
    Serial.println(p.y);
    delay(30);
}

void printpoint_calibed(TS_Point p){
    Serial.print("Pressure = ");
    Serial.print(p.z);
    Serial.print(", x = ");
    Serial.print(convert_x(p.x));
    Serial.print(", y = ");
    Serial.print(convert_y(p.y));
    delay(30);
    Serial.println();
}


void calibration (void) {
  drawCrossPoint(CALIBRATION_POINT, CALIBRATION_POINT, ILI9341_RED);
  drawCrossPoint(tft.width()-CALIBRATION_POINT, tft.height()-CALIBRATION_POINT, ILI9341_WHITE);

  Serial.println("Calibration: touch red star:");
  
  uint8_t count = 0;
  float sum_x = 0.0;
  float sum_y = 0.0;
  while(true) {
    while(ts.touched() && count<10) {
      TS_Point point = ts.getPoint();
      printpoint_raw(point);
      sum_x += point.x;
      sum_y += point.y;
      count++;
    }
    if(count==10) break;
  }
  calibration_x1 = sum_x / 10.0;
  calibration_y1 = sum_y / 10.0;
  Serial.print("Calibration (x1,y1)=(");
  Serial.print(calibration_x1);
  Serial.print(",");
  Serial.print(calibration_y1);
  Serial.println(")");
  
  delay(1000);
  
  drawCrossPoint(CALIBRATION_POINT, CALIBRATION_POINT, ILI9341_WHITE);
  drawCrossPoint(tft.width()-CALIBRATION_POINT, tft.height()-CALIBRATION_POINT, ILI9341_RED);

  count = 0;
  sum_x = 0.0;
  sum_y = 0.0;
  while(true) {
    while(ts.touched() && count<10) {
      TS_Point point = ts.getPoint();
      printpoint_raw(point);
      sum_x += point.x;
      sum_y += point.y;
      count++;
    }
    if(count==10) break;
  }
  float calibration_x2 = sum_x / 10.0;
  float calibration_y2 = sum_y / 10.0;
  Serial.print("Calibration (x2,y2)=(");
  Serial.print(calibration_x2);
  Serial.print(",");
  Serial.print(calibration_y2);
  Serial.println(")");

  delta_x = (tft.width()-CALIBRATION_POINT*2)/ (calibration_x2-calibration_x1);
  delta_y = (tft.height()-CALIBRATION_POINT*2)/ (calibration_y2-calibration_y1);

  drawCrossPoint(CALIBRATION_POINT, CALIBRATION_POINT, ILI9341_BLACK);
  drawCrossPoint((tft.width()-CALIBRATION_POINT), (tft.height()-CALIBRATION_POINT), ILI9341_BLACK);

  char s[64];
  Serial.print("Cal(");
  Serial.print(calibration_x1);
  Serial.print(",");
  Serial.print(calibration_y1);
  Serial.println(")");
  Serial.print("Delta(");
  Serial.print(delta_x*1000.0);
  Serial.print(",");
  Serial.print(delta_y*1000.0);
  Serial.println(") at x1000");
}

void drawCrossPoint(uint16_t x, uint16_t y,uint16_t color){
  tft.drawLine(x-5,y,x+5,y,color);
  tft.drawLine(x,y-5,x,y+5,color);
  tft.drawCircle(x,y,2,color);
}

int check_key1(int val_old) {
  int status=SKIP;
  uint8_t bufptr=0;
  boolean loopstatus=true;
  char *key;
  int ikey;
  int val;

  Serial.print("Keyboard:status ");
  Serial.println(status);
  
  do {
    if((ikey=touched2(key1_array,12)) >= 0) {
      key = key1_array[ikey]->key;
      status = key1_array[ikey]->status;
    }
    switch (status) {
    case KEYIN:
      tft.setCursor(bufptr*12,0);
      tft.print(key);
      strbuf[bufptr]=*key;
      bufptr++;
      status = SKIP;
      break;
    case KEYCLR:
      tft.fillRect(0,0,bufptr*12,14,ILI9341_BLACK);
      bufptr=0;
      status=SKIP;
      break;
    case KINFIN:
      strbuf[bufptr]='\0';
      bufptr++;
      loopstatus=false;
      break;
    case SKIP:
    default:
      break;
    }
  }
  while(loopstatus);
  
  if(bufptr<=1) {
    val = val_old;
  } else {
    val = atoi(strbuf);
  }
  return val;
}

int touched2(BUTTON **key_array,int narray)
{
  TS_Point point;
  int ikey=-1;
  boolean valid=false;
  if(ts.touched()) {
    point = ts.getPoint();
    printpoint_raw(point);
    printpoint_calibed(point);
      for(uint8_t i=0;i<narray;i++) {
        if(is_in_area(key_array[i],point)) {
          valid = true;
          ikey = i;
          draw_button(key_array[ikey],ILI9341_RED);
        }
      }
  } else {
    return -1;
  }

  if(ikey<0)
    return -1;

  while(ts.touched()) {
    point = ts.getPoint();
    if(!is_in_area(key_array[ikey],point)) {
      valid = false;
      draw_button(key_array[ikey],ILI9341_WHITE);
    }
  }

  draw_button(key_array[ikey],ILI9341_WHITE);
  if(valid)
    return ikey;
  else
    return -1;
}

void setup()
{
  int i;
  
  Serial.begin(115200);
  Wire.begin();
  Serial.println();
  Serial.println();
#ifdef USE_BT_SERIAL
  SerialBT.begin("BTThermoMonitor");
#endif

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  do {
    if(WiFi.status() == WL_CONNECTED)
      {
	Serial.println("");
	WiFi_valid = true;
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	ntp.udpSetup();
	break;
      } else {
      Serial.print(".");
    }
    i++;
  } while(i<WiFiAttempts);
  
#ifdef ENABLE_WEBSERVER
  if(WiFi_valid==true)
    WifiServerinit();
#endif
  
  if(!RTC.begin())
    Serial.println("RTC not working. Skipping...");
  else
    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running! Reset date");
    // following line sets the RTC to the date & time this sketch was compiled
      //    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
      if(WiFi_valid)
	setRTC();
    }

  Serial.println("MAX6675");
  // wait for MAX chip to stabilize
  Serial.print("Initializing sensor...");
  delay(500);
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");

  // web server and SD card
#ifdef ENABLE_WEBSERVER
  if(WiFi_valid)
    setSDStatus(sd.init());
  else
    sd.init();
#else
  sd.init();
#endif

  // start TFT
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);

  // start Touchscreen
  ts.begin();
  ts.setRotation(2);

  // do calibration of Touchscreen
  calibration();

  if(WiFi_valid) {
    tft.setTextSize(2);
    tft.setCursor(0,0);
    tft.print("IP address: ");
    tft.println(WiFi.localIP());
  }
  
  drawAxis();
  draw_settingButton();
}

void loop()
{
  int status = LOOP;
  double c;
  DateTime now;

#ifdef ENABLE_WEBSERVER
  handleClient();
#endif

  // 約50日ごとにおこるオーバーフロー時はmillsが0になるので引き算の結果はマイナス
  // とりうる値の範囲を考慮して割り算した結果同士を比較 (4,294,967,295)
  if((signed long)(millis()/1000000 - whenCountStarted/1000000) < 0){whenCountStarted = millis();}
  if((millis() - whenCountStarted) > period)
  {
    whenCountStarted = millis();

    // 時間を取得
    /*
    if((ntpAccsessTimes==0) && (ntp.getTime(&unixtime)))
    {
      ntpAccsessTimes++;
      if(ntpAccsessTimes >= 480){ ntpAccsessTimes = 0;} // 48回に1回のアクセスとする: 8時間に1回
    } else {
      //時刻取得に失敗 or 取得タイミングでなかったら前回の値から測定間隔分だけ秒数を足す
      unixtime += period/1000;
    }
    time_t t = unixtime + (UTC_TOKYO * 60 * 60); // 日本標準時に調整
    sprintf(date_ym, "%04d-%02d", year(t), month(t));
    sprintf(date_mdhm, "%2d/%02d-%02d:%02d", month(t), day(t),hour(t), minute(t));
    */
    now = RTC.now();
    sprintf(date_ym, "%04d-%02d", now.year(), now.month());
    sprintf(date_mdhm, "%2d/%02d-%02d:%02d", now.month(), now.day(), now.hour(), now.minute());
    
    Serial.println(date_ym);
    Serial.println(date_mdhm);

    tft.setTextSize(2);
    tft.fillRect(12*14,0,12*(14+10),14,ILI9341_BLACK);
    tft.setCursor(12*14, 0);tft.print(date_mdhm);

    // センサからデータを取得

    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(0,0);
    tft.fillRect(0,0,12*14,14,ILI9341_BLACK);
    c = thermocouple.readCelsius();
    if (isnan(c)) {
      Serial.println("Something wrong with thermocouple!");
      tft.setTextSize(1);
      tft.println("Something wrong with thermocouple!");     
    } else {
      Serial.print("T = ");
      Serial.print(c);
      Serial.println("C");
      tft.print("T = ");
      tft.print(c);
      tft.print("C");
    }

#ifdef USE_BT_SERIAL
    SerialBT.print(date_mdhm);
    SerialBT.print("\t");
    SerialBT.println(c);
#endif
    // tft.setCursor(0,17);
    // tft.setCursor(12*17,0);tft.print(c1);
    // double c1=thermocouple.readInternal();
    // Serial.print("Internal Temp = ");
    // Serial.println(c1);
//    tft.print("Internal Temp = ");
//    tft.setCursor(0,17);tft.fillRect(0,17,12*17,17+14,ILI9341_BLACK);

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
//    dataStream += String(c1);
    dataStream += String('0');
    sd.recordData(fileName,dataStream);
  }

  int ikey;
  char *key;
  if((ikey = touched2(key0_array,2))>=0) {
    Serial.print("ikey=");
    Serial.println(ikey);
    key= key0_array[ikey]->key;
    status = key0_array[ikey]->status;
    Serial.println(status);
  }

  int num;
  switch (status) {
  case LOOP:
    break;
  case SETTING:
      Serial.println("Status: INPKEY");
    delay(100);
    touch_pressed = false;
    tft.fillScreen(ILI9341_BLACK);
    draw_keys1();
    num = check_key1(203);
    Serial.println(num);
    tft.fillScreen(ILI9341_BLACK);
    draw_keys0();
    status = LOOP;
    break;        
  default:
    break;
  }
  //  delay(50);
}

void checkFile(char *fileName)
{
  File dataFile = SD.open(fileName, FILE_WRITE);
  if(!dataFile) {
    SD.open(fileName);
  } 
}

void setRTC()
{
  if((ntp.getTime(&unixtime)))
  {
    time_t t = unixtime + (UTC_TOKYO * 60 * 60); // 日本標準時に調整
    RTC.adjust(DateTime(year(t), month(t), day(t), hour(t), minute(t), second(t)));
  }
}
