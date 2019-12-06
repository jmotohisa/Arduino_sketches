// GPS Logger version 5.0

/*
  ESP8266 ESP-12E
  with
   u-Blox GM-8013T or GT-902PMGG (u-Blox 8)
   SDcard
   M096P4BL (SSD1306) 
*/

#include <ESP8266WiFi.h>

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include <SPI.h>
#include "SdFat.h"

using namespace sdfat;

SdFat SD;
//#include <MsTimer2.h>
#include <Ticker.h>

//#include "X11fixed7x14.h"
#include "font.h"

/* Circuit Connction */

/*
 SD Card  | ESP8266  | Label
-------- | -------- | -------
MOSI     | GPIO 13  | D7
MISO     | GPIO 12  | D6
CLK      | GPIO 14  | D5
CS       | GPIO 15  | D8
*/
#define SD_MOSI D7
#define SD_MISO D6
#define SD_CLK D5
#define SD_CHIP_SELECT D8

/*
M096PBL  | ESP8266  | Label
SCL      | GPIO 5   | D1
SDA      | GPIO 4   | D2
*/
#define OLED_I2C_ADDRESS 0x3C
#define OLED_SDA D2
#define OLED_SCL D1

/*
GT-902PMGG | ESP8266  | Label
 TTL output | GPIO 3 |  RX
TTL input  | GPIO 1 |  TX
*/

/*
LED      | GPIO 2   | (D4)
*/
// LED_BUILTIN

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

#define TimeZone (9)
#define SW_THRESHOLD 50

#define BUFSIZE 100
#define BUF2SIZE 12

// initialize the library with the numbers of the interface pins
//SoftwareSerial gps(8, 9); // RX, TX

// SSD1306Wire display(OLED_I2C_ADDRESS, OLED_SDA, OLED_SCL);
// Define proper RST_PIN if required.
#define RST_PIN -1

// Initialize the OLED display using i2c
SSD1306AsciiWire oled;

// Ticker (timer)
Ticker ticker ;

SdFile logFile;

int gpsHour, gpsMin, gpsSec;
int gpsDay, gpsMonth, gpsYear;

char strbuf[BUFSIZE];
char s1[BUF2SIZE];
char filename[13];
char substr[7];
bool fileEnable;
bool runMode;
bool logFileOpened;

void setup() {

// turn off wifi
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

//
  pinMode(LED_BUILTIN, OUTPUT) ;      // LEDに接続
//  pinMode(SW_PIN_NO, INPUT_PULLUP ) ; // SW に接続し内部プルアップに設定
    digitalWrite(LED_BUILTIN, HIGH); // turn off (active LOW)

  Wire.begin();
  Wire.setClock(400000L);
// initialize OLED Display M096P4BL
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
#endif // RST_PIN >= 0
  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(X11fixed7x14);
//  oled.setFont(System5x7);
  oled.clear();
  oled.setCursor(0,0);
  oled.print("Initalizing...");

  // initialize Hardware Serial forGPS
  //  Serial.begin(115200);
  Serial.begin(9600);
  delay(5000);
  //  Serial.begin(9600); // Hardware Serial
  // configure output of GM - 8013T
  configure_GP8013T();
  
  oled.setCursor(0,2);
  oled.print("GPS ready");
  
  delay(1000);
  
// initialize SD card
  filename[0]='\0';
  oled.setCursor(0,0);
  oled.clear();
  oled.print("Init SD card");
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (!SD.begin(SD_CHIP_SELECT,SD_SCK_MHZ(10))) {
    // fileEnable = checkSDFile();
    fileEnable = true;
  } else {
    oled.setCursor(0,6);
    oled.println("SD begin failed.");
    fileEnable=false;
  }
  logFileOpened=false;
  delay(2000);
  
  oled.setCursor(0,0);
  oled.clearToEOL();

// enable timer2 (timer library)
//  MsTimer2::set(60000, flushSD);
//  MsTimer2::start();
  ticker.attach_ms(60000, flushSD);

  runMode = 0;
}

void loop()
{
  int swStatus;

  swStatus = analogRead(A0);
  if(fileEnable) {
    if (runMode == 1 && swStatus <= SW_THRESHOLD) {
      runMode = 0; // logging turn off
      digitalWrite(LED_BUILTIN, HIGH); // turn off (active LOW)
      if (logFileOpened == true) {
        logFile.close();
        logFileOpened = false;
      }
    } else if (runMode == 0 && swStatus > SW_THRESHOLD) { // logging turn on
      runMode = 1;
      if (logFileOpened == false) {
        if(strlen(filename)==0) {
          strcpy(filename,"GPtemp.txt");
        }
        if(!logFile.open(filename, FILE_WRITE)) {
          oled.print("Can't open logfile");
          fileEnable = false;
          logFileOpened=false;
         } else {
           logFileOpened=true;
         }
      }           
      digitalWrite(LED_BUILTIN, LOW); // turn on (active LOW)
    }
  }
  doLogging();
}

void doLogging()
{
  char utcTime[10], utcDate[7];
  char latitude[11], longtude[12];
  char NS[2], WE[2];
  char statGPS[2];
//  char GPSStr[7];
  char localTime0[9], localDate0[9];
  int offset;
//  int xpos;

  if (Serial.available()) {  // if recived serial signal
//    digitalWrite(LED_BUILTIN,LOW);
    recvStr();   // read serial data to string buffer
    if (logFileOpened == true) {
      logFile.print(strbuf);
//      logFile.flush();
    } else {
  }
//    digitalWrite(LED_BUILTIN,HIGH);

    // Display date/time/latitude/longitude
    offset = strip_NMEA(0, 1);
    //      strcpy(GPSStr,s1);
    if (strcmp(s1, "$GNRMC") == 0) { // if RMC line
      offset = strip_NMEA(offset, 1); // utcTime
      strcpy(utcTime, s1);
      offset = strip_NMEA(offset, 1); // status
      strcpy(statGPS, s1);
      offset = strip_NMEA(offset, 1); // latitue
      strcpy(latitude, s1);
      offset = strip_NMEA(offset, 1); // N/W
      strcpy(NS, s1);
      offset = strip_NMEA(offset, 1); // longitude
      strcpy(longtude, s1);
      offset = strip_NMEA(offset, 1); // W/E
      strcpy(WE, s1);
      offset = strip_NMEA(offset, 3); // utcDate
      strcpy(utcDate, s1);

      gpsDate(utcDate);
      gpsTime(utcTime);
      UCTtoLT();
      sprintf(localDate0, "%02d/%02d/%02d", gpsYear,gpsMonth,gpsDay);
      sprintf(localTime0, "%02d:%02d:%02d", gpsHour,gpsMin,gpsSec);
      sprintf(filename, "GP%02d%02d%02d.txt", gpsYear,gpsMonth,gpsDay);

      oled.setCursor(0,0);
      oled.print(localTime0);
      oled.setCursor(64,0);
      oled.print(localDate0);

      if (strchr(statGPS, 'A')) {
        oled.setCursor(0,2);
        oled.print(NS);
        oled.print(latitude);
        oled.setCursor(0,4);
        oled.print(WE);
        oled.print(longtude);
      } else {
        oled.setCursor(0,2);
        oled.print("GPS invalid");
      }
	  
    }
  }
}

// recieve string from GPS
void recvStr()
{
  int i = 0;
  char c;
  while (1) {
    if (Serial.available()) {
      c = Serial.read();
      strbuf[i] = c;
      if (c == '\n') break;
      i++;
    }
  }
  i++;
  strbuf[i] = '\0';  // \0: end of string
//  if(digitalRead(SW_PIN_NO)){
/*
    strncpy(substr,strbuf,6);
    substr[6]='\0';
    oled.setCursor(0,6);
    oled.print(substr);
    */
    oled.setCursor(0,6);
    oled.print(strbuf);
    
//  }
}

// get info from NMEA sentence
unsigned int strip_NMEA(unsigned int offset, int count)
{
  char *str0, *s0;
  int i, len;

  for (i = 0; i < count; i++) {
    str0 = strbuf+offset;
    s0 = strchr(str0, ',');
    len = strlen(str0) - strlen(s0);
    strncpy(s1, str0, len); // len < BUF2SIZE +1 assumed
    s1[len] = '\0';
    offset += strlen(s1) + 1;
  }

  return offset;
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
    strcpy(filename,"GPtemp.txt");
  oled.println("Opening log file");

// open log file
  if (!logFile.open(filename, FILE_WRITE)) {
    oled.setCursor(0,6);
    oled.print("Can't open logfile");
    return false;
  }
  oled.clear();
  oled.setCursor(0,0);
  oled.print("Log file opened.");
  oled.setCursor(0,6);
  oled.clearToEOL();
  oled.setCursor(64,6);
  oled.print(filename);
  logFile.close();
  SdFile::dateTimeCallback( &dateTime );
  return true;
}

// send NMEA command with checksum to gps (hardware serial)
void send_nmea_command(const char *p)
{
  uint8_t checksum = 0;
  Serial.print('$');
  do {
    char c = *p++;
    if (c) {
      checksum ^= (uint8_t)c;
      Serial.print(c);
    }
    else {
      break;
    }
  }
  while (1);
  Serial.print('*');
  Serial.println(checksum, HEX);
  //  Serial.print("\n\r");
}

void send_PUBX_packet(const char *p)
{
  uint8_t checksum = 0;
  Serial.print('$');
  do {
    char c = *p++;
    if (c) {
      checksum ^= (uint8_t)c;
      Serial.print(c);
    }
    else {
      break;
    }
  }
  while (1);
  Serial.print('*');
  Serial.println(checksum, HEX);
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
void gpsTime0(uint16_t UTC)
{
  gpsHour = int(UTC / 10000);
  gpsMin = int(UTC % 10000 / 100);
  gpsSec = UTC % 100;
}
// Date ->(gpsYear,gpsMonth,gpsYear)
void gpsDate0(uint16_t dateRead)
{
  gpsDay = int(dateRead / 10000);
  gpsMonth = int(dateRead % 10000 / 100);
  gpsYear = dateRead % 100; //last 2 digits, e.g. 2013-> 13
}

void gpsTime(const char *utcTime)
{
  char s[3];
  mysubstr(s,utcTime,0,2);
  gpsHour=atoi(s);
  mysubstr(s,utcTime,2,2);
  gpsMin=atoi(s);
  mysubstr(s,utcTime,4,2);
  gpsSec=atoi(s);
}

void gpsDate(const char *utcDate)
{
  char s[3];
  mysubstr(s,utcDate,4,2);
  gpsYear = atoi(s);
  mysubstr(s,utcDate,2,2);
  gpsMonth = atoi(s);
  mysubstr(s,utcDate,0,2);
  gpsDay = atoi(s);
}
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

int mysubstr(char *t, const char *s, int pos, unsigned int len )
{
//    if( pos < 0 || len < 0 || len > strlen(s) )
    if( pos < 0 || len > strlen(s) )
        return -1;
    for( s += pos; *s != '\0' && len > 0; len-- )
        *t++ = *s++;
    *t = '\0';
    return 0;
}

bool setFileName()
{
//  char GPSStr[7];
  char utcTime[10], utcDate[7];
  unsigned int offset;

  if (Serial.available()) {  // if recived serial signal
//    digitalWrite(LED_BUILTIN,LOW);
    recvStr();   // read serial data to string buffer
    offset = strip_NMEA(0, 1);
    if (strcmp(s1, "$GNRMC") == 0) { // if RMC line
      offset = strip_NMEA(offset, 1); // utcTime
      strcpy(utcTime, s1);
      offset = strip_NMEA(offset, 8); // utcDate
      strcpy(utcDate, s1);
      if (strlen(utcDate) == 6 ) { // if date utcDate is valid
        gpsDate(utcDate);
        gpsTime(utcTime);
        UCTtoLT();
        sprintf(filename, "GP%02d%02d%02d.txt", gpsYear,gpsMonth,gpsDay);
        return true;
      }
    }
//    digitalWrite(LED_BUILTIN,HIGH);
  }
  return false;
}

void flushSD()
{
  if (logFileOpened == true) {
    digitalWrite(LED_BUILTIN, HIGH); // actiate LOW
    logFile.flush();
    oled.setCursor(0,6);
    oled.clearToEOL();
    oled.print(filename);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW); // activate LOW
  }
}
