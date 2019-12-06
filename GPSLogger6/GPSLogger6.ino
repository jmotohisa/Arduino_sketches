// GPS Logger version 6.0

// TinyGPS version

/*
  Arduino or Bule pill (STM32duino)
  with
   u-Blox GM-8013T or GT-902PMGG (u-Blox 8)
   SDcard
   M096P4BL (SSD1306)
*/

/* note
  Arudino - SDcard - Blue Pill
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila   - PA7
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila   - PA6
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila    - PA5
 ** CS - depends on your SD card shield or module. ->10  - PA4
*/

/* Arduino  (with software serial) - GM-8013T - Blue pill (Serial2/UART2)
    RX: 8 - 3 (TTL output) - PA3
    TX: 9 - 4 (TTL input) -  PA2
*/

/* Arudio - M096PBL - Blue Pill
    A5 - SCL - B6 
    A4 - SDA - B7
  */

/* flow
  初期化ステップ1(シリアルポート、ソフトウェアシリアル、OLEDの初期化)
  初期化ステップ2(GPSの設定：データの受信間隔・GPSより出力/ログするセンテンスの設定)
  初期化ステップ3(SD・ファイルの初期化)
  データを受信し、GPRMCもしくはGNRMCセンテンスのチェック→有効なRMCセンテンスならば日付を得る
  日付、および既存ファイルと重複しないようファイル名を決定
  ファイル作成日を指定していったんファイルをopen→close

  loop
  データ受信
  LED点灯
  ファイルが有効ならば
    ファイルをopen
    データをファイルに記録(シリアルに出力)
    ファイルをclose
    LED消灯
  GPRMCもしくはGNRMCセンテンスのチェック、RMCセンテンスならば
    有効なRMCセンテンスならば
      validのindicator をOLEDに表示
      緯度・経度・日付・時刻を得て、OLEDに情報を表示
    でなければ
      invalid dataであることの表示
  loop end

*/

// uncomment if debug and use serial
#define DEBUG_SERIAL

// uncomment if use STM32F103 (blue pill) with STM32duino bootoloader
#define STM32duino

//#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#ifndef STM32duino
#include <SoftwareSerial.h>
#endif
//#include <MsTimer2.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define TimeZone (9)
#define logPeriod 5000

#ifndef STM32duino
#define LED_PIN_NO 7
#define SW_PIN_NO 6
#define LED_ON HIGH
#define LED_OFF LOW
#define SD_CHIP_SELECT 10
#else
#define LED_PIN_NO PC13
#define SW_PIN_NO 6
#define LED_ON LOW
#define LED_OFF HIGH
#define SD_CHIP_SELECT PA4
#endif

#define BUFSIZE 100
#define BUF2SIZE 12

TinyGPSPlus gps;

// initialize the library with the numbers of the interface pins
#ifndef STM32duino
  SoftwareSerial Serial2(8, 9); // RX, TX
//#define GPS gps
#else
//#define GPS Serial2
#endif

// setup u8g object
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

File logFile;

uint8_t gpsHour, gpsMin, gpsSec;
uint16_t gpsYear;
uint8_t gpsDay, gpsMonth;

char strbuf[BUFSIZE];
char s1[BUF2SIZE];
char filename[13];
char substr[7];
bool fileEnable;
bool runMode;
bool logFileOpened;

void setup() {
#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  delay(1000);
  Serial.println("GPS Logger Start");
#endif

  pinMode(LED_PIN_NO, OUTPUT) ;      // LEDに接続
  pinMode(SW_PIN_NO, INPUT_PULLUP ) ; // SW に接続し内部プルアップに設定

  // initialize OLED Display M096P4BL
  Wire.begin();
  Wire.setClock(400000L);
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(X11fixed7x14);
//  oled.setFont(System5x7);
  oled.clear();
  oled.setCursor(0,0);

  // initialize Hardware Serial and GPS
  Serial2.begin(9600);
  delay(1000);
  // configure output of GM - 8013T
  configure_GP8013T();
  
  oled.print("GPS ready");
#ifdef DEBUG_SERIAL
  Serial.println("GPS ready");
#endif    

  delay(1000);
  
// initialize SD card
  filename[0]='\0';
  oled.setCursor(0,0);
  oled.clear();
  oled.print("Init SD card");
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (SD.begin(SD_CHIP_SELECT)) {
//    fileEnable = checkSDFile();
  } else {
    oled.setCursor(0,6);
//    oled.clearToEOL();
    oled.println("SD begin failed.");
    fileEnable=false;
  }
  logFileOpened=false;
  delay(2000);

  oled.setCursor(0,0);
  oled.clearToEOL();

  // enable timer2
//  MsTimer2::set(60000, flushSD);
//  MsTimer2::start();

  runMode = 0;
}

void loop()
{
  int swStatus;

  swStatus = digitalRead(SW_PIN_NO);
  if (runMode == 1 && swStatus == LOW) {
    runMode = 0; // logging turn off
    digitalWrite(LED_PIN_NO, LED_OFF);
    if (logFileOpened == true) {
      logFile.close();
      logFileOpened = false;
    }
  } else if (runMode == 0 && swStatus == HIGH) { // logging turn on
    runMode = 1;
    if (logFileOpened == false) {
      if(!fileEnable) {
//        fileEnable = checkSDFile();
      }
      if(fileEnable) {
        logFile = SD.open(filename, FILE_WRITE);
      }
      
      if(!logFile || !fileEnable) {
        oled.setCursor(0,6);
        oled.print("Can't open logfile");
      } else {
        logFileOpened=true;
        digitalWrite(LED_PIN_NO, LED_ON);
      }
    }
  }
  doLogging();
}

void doLogging()
{
  //  char utcTime[10], utcDate[7];
  double latitude,longtude;
  char localTime0[9], localDate0[9];

  smartDelay(logPeriod);
  if (millis() > logPeriod*5 && gps.charsProcessed() < 10) {
#if DEBUG_SERIAL
    Serial.println(F("No GPS data received: check wiring"));
#endif
	oled.setCursor(0,0);
	old.print("GPS invalid");
  }

  if (gps.location.isValid()) {
	latitude = gps.location.lat();
	longtude = gps.location.lng();
  } else {
	latitude = 0;
	longtude = 0;
	oled.setCursor(0,2);
	oled.print("GPS invalid");
#if DEBUG_SERIAL
	Serial.println("GPS Invalid");
#endif
  }
  if(gps.date.isValid()) {
	gpsDay = gps.date.day();
	gpsMonth = gps.date.month();
	gpsYear = gps.date.year();
  } else {
	gpsDay = 1;
	gpsMonth = 1;
	gpsYear = 2000;
  }
  if(gps.time.isValid()) {
	gpsHour = gps.time.hour();
	gpsMin = gps.time.minute();
	gpsSec = gps.time.second();
  } else {
	gpsHour = 0;
	gpsMin = 0;
	gpsSec = 0;
  }
  if (logFileOpened == true) {
	log_to_file();
	logFile.flush();
  }
  
  // output data to Serial
#ifdef DEBUG_SERIAL
  Serial.print(latitude, 6);
  Serial.print(F(","));
  Serial.print(longtude, 6);
  
  Serial.print(gpsMonth);
  Serial.print(F("/"));
  Serial.print(gpsDay);
  Serial.print(F("/"));
  Serial.print(gpsYear);
  Serial.print(F(" "));
  
  if (gpsHour < 10) Serial.print(F("0"));
  Serial.print(gpsHour);
  Serial.print(F(":"));
  if (gpsMin < 10) Serial.print(F("0"));
  Serial.print(gpsMin);
  Serial.print(F(":"));
  if (gpsSec < 10) Serial.print(F("0"));
  Serial.println(gpsSec);
#endif
  
  // Display GPS data
  oled.setCursor(0,2);
  if(latitude>0) {
	oled.print("N");
  } else {
	oled.print("S");
  }
  oled.print(latitude);
  oled.setCursor(0,4);
  if(longtude>0) {
	oled.print("E");
  }  else {
	oled.print("W");
  }
  oled.print(longtude);
  
  UCTtoLT();
  // Display date/time
  sprintf(localDate0, "%02d/%02d/%02d", gpsYear,gpsMonth,gpsDay);
  sprintf(localTime0, "%02d:%02d:%02d", gpsHour,gpsMin,gpsSec);
  oled.setCursor(0,0);
  oled.print(localTime0);
  oled.setCursor(64,0);
  oled.print(localDate0);
}

bool checkSDFile()
{
  // check log file
  int i, loopmax = 100;

  oled.setCursor(0,6);
  for (i = 0; i < loopmax; i++) {
    if(setFileName()) 
      break;
    if(i%10==0)
    {
      oled.setCursor(0,6);
      oled.clearToEOL();
      oled.print(i/10);
      oled.print(':');
    }
    oled.print(i%10);
    delay(500);
  }
  if(strlen(filename)==0) 
    strcpy(filename,"GPtemp.csv");
  oled.println("Opening log file");
#ifdef DEBUG_SERIAL
  Serial.print("logfile: ");
  Serial.print(filename);
  Serial.println(" opened");
#endif

// open log file
  logFile = SD.open(filename, FILE_WRITE);
  if (!logFile) {
    oled.setCursor(0,6);
    oled.print("Can't open logfile");
    return false;
  }
  oled.clear();
  oled.setCursor(0,0);
  oled.print("Log file opened.");
  oled.setCursor(0,6);
  oled.clearToEOL();
  oled.print(filename);
  logFile.close();
  SdFile::dateTimeCallback( &dateTime );
  return true;
}

// send NMEA command with checksum to gps (hardware serial)
void send_nmea_command(const char *p)
{
  uint8_t checksum = 0;
  Serial2.print('$');
  do {
    char c = *p++;
    if (c) {
      checksum ^= (uint8_t)c;
      Serial2.print(c);
    }
    else {
      break;
    }
  }
  while (1);
  Serial2.print('*');
  Serial2.println(checksum, HEX);
  //  gps.print("\n\r");
}

void send_PUBX_packet(const char *p)
{
  uint8_t checksum = 0;
  Serial2.print('$');
  do {
    char c = *p++;
    if (c) {
      checksum ^= (uint8_t)c;
      Serial2.print(c);
    }
    else {
      break;
    }
  }
  while (1);
  Serial2.print('*');
  Serial2.println(checksum, HEX);
}

// $GNRMC, $GNVTG, $GNGGA, $GPGSV, $GLGSV, $GNGLL, $GNGSA
void configure_GP8013T()
{
  // set NMEA sentence output rate
  // "$PUBX,40", followed by
  // msgID, 
  // output rate on DDC
  // output rate on USART1
  // output rate on USART2
  // output rate on USB
  // output rate on SPI
  // 0 (reserved)
  // and "*" + checksum + CRLF
  send_PUBX_packet("PUBX,40,RMC,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,VTG,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GGA,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GSV,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GLL,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GSA,0,5,0,0,0,0");
}

void dateTime(uint16_t* date, uint16_t* time)
{
  // GPSやRTCから日付と時間を取得
  // FAT_DATEマクロでフィールドを埋めて日付を返す
  *date = FAT_DATE(gpsYear + 2000, gpsMonth, gpsDay);

  // FAT_TIMEマクロでフィールドを埋めて時間を返す
  *time = FAT_TIME(gpsHour, gpsMin, gpsSec);
}

// UTC -> (gpsHour, gpsMin, gpsSec)
/* void gpsTime0(uint16_t UTC) */
/* { */
/*   gpsHour = int(UTC / 10000); */
/*   gpsMin = int(UTC % 10000 / 100); */
/*   gpsSec = UTC % 100; */
/* } */
// Date ->(gpsYear,gpsMonth,gpsYear)
/* void gpsDate0(uint16_t dateRead) */
/* { */
/*   gpsDay = int(dateRead / 10000); */
/*   gpsMonth = int(dateRead % 10000 / 100); */
/*   gpsYear = dateRead % 100; //last 2 digits, e.g. 2013-> 13 */
/* } */

/* void gpsTime(const char *utcTime) */
/* { */
/*   char s[3]; */
/*   mysubstr(s,utcTime,0,2); */
/*   gpsHour=atoi(s); */
/*   mysubstr(s,utcTime,2,2); */
/*   gpsMin=atoi(s); */
/*   mysubstr(s,utcTime,4,2); */
/*   gpsSec=atoi(s); */
/* } */

/* void gpsDate(const char *utcDate) */
/* { */
/*   char s[3]; */
/*   mysubstr(s,utcDate,4,2); */
/*   gpsYear = atoi(s); */
/*   mysubstr(s,utcDate,2,2); */
/*   gpsMonth = atoi(s); */
/*   mysubstr(s,utcDate,0,2); */
/*   gpsDay = atoi(s); */
/* } */
/*
  // Latitude/Longitude
  void gpsLatLong(int lat1, int lat2, int long1, int long2)
  {
  gpsLat = int(lat1/100) + (lat1%100)/60.0 + float(lat2)/10000.0/60.0;
  gpsLong = int(long1/100) + (long1%100)/60.0 + float(long2)/10000.0/60.0;
  }
*/

void UCTtoLT()
{
//  int leap = (gpsYear % 4) * 4 / gpsYear - (gpsYear % 100) * 100 / gpsYear + (gpsYear % 400) * 400 / gpsYear ;
  int leap = (gpsYear / 4) * 4 / gpsYear ;
  int DaysAMonth[12] = {31, 28 + leap, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  gpsHour += TimeZone;
  if (gpsHour < 0)
  {
    gpsHour += 24;
    gpsDay -= 1;
    if (gpsDay < 1)
    {
      if (gpsMonth == 1)
      {
        gpsMonth = 12;
        gpsYear -= 1;
      }
      else
      {
        gpsMonth -= 1;
      }
      gpsDay = DaysAMonth[gpsMonth - 1];
    }
  }
  if (gpsHour >= 24)
  {
    gpsHour -= 24;
    gpsDay += 1;
    if (gpsDay > DaysAMonth[gpsMonth - 1])
    {
      gpsDay = 1;
      gpsMonth += 1;
      if (gpsMonth > 12) {
        gpsYear += 1;
        gpsMonth = 1;
      }
    }
  }
}

/* int mysubstr(char *t, const char *s, int pos, unsigned int len ) */
/* { */
/* //    if( pos < 0 || len < 0 || len > strlen(s) ) */
/*     if( pos < 0 || len > strlen(s) ) */
/*         return -1; */
/*     for( s += pos; *s != '\0' && len > 0; len-- ) */
/*         *t++ = *s++; */
/*     *t = '\0'; */
/*     return 0; */
/* } */

bool setFileName()
{
  if (Serial2.available()) {  // if recived serial signal
	//    digitalWrite(LED_PIN_NO,LED_ON);
    gps.decode(Serial2.read());
	if (gps.date.isValid())
	  {
		gpsMonth=gps.date.month();
		gpsYear=gps.date.year() % 100;
		gpsDay=gps.date.day();
        UCTtoLT();
        sprintf(filename, "GP%02d%02d%02d.csv", gpsYear,gpsMonth,gpsDay);
        return true;
      }
	//    digitalWrite(LED_PIN_NO,LED_OFF);
  }
  return false;
}

void flushSD()
{
  if (logFileOpened == true) {
    digitalWrite(LED_PIN_NO, LED_OFF);
    logFile.flush();
    oled.setCursor(0,6);
    oled.clearToEOL();
    oled.print(filename);
    delay(500);
    digitalWrite(LED_PIN_NO, LED_ON);
  }
}

void log_to_file()
{
  logFile.print(gpsYear);  logFile.print(",");
  logFile.print(gpsMonth);  logFile.print(",");
  logFile.print(gpsDay);  logFile.print(",");
  logFile.print(gpsHour);  logFile.print(",");
  logFile.print(gpsMin);  logFile.print(",");
  logFile.print(gpsSec);  logFile.print(",");
  logFile.print(gps.location.lat());  logFile.print(",");
  logFile.print(gps.location.lng());  logFile.print(",");
  logFile.print(gps.altitude.meters()); logFile.print(",");
  logFile.print(gps.course.deg());logFile.print(",");
  logFile.print(gps.speed.kmph());
  logFile.println("");

}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}
