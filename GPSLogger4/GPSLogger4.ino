// GPS Logger version 3.0

/* with
   u-Blox GM-8013T
   SDcard
   M096P4BL
*/

/* note
  Arudino-SDcard
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module. ->10
*/

/* Arduino - GM-8013T (with Software Serial), gps(8,9)
    RX: 8 - 3 (TTL output)
    TX: 9 - 4 (TTL input)
*/

/* Arudio - M096PBL
    A5 - SCL
    A4 - SDA
  v*/

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
//#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "SSD1306Ascii.h"
//#include "SSD1306AsciiWire.h"
#include "SSD1306AsciiAvrI2c.h"

#define SD_CHIP_SELECT 10
#define TimeZone (9)
#define SW_PIN_NO 6
#define LED_PIN_NO 7
#define BUFSIZE 90
#define BUF2SIZE 12

// initialize the library with the numbers of the interface pins
SoftwareSerial gps(8, 9); // RX, TX

// setup u8g object
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

//SSD1306AsciiWire oled;
SSD1306AsciiAvrI2c oled;

File logFile;

int gpsHour, gpsMin, gpsSec;
int gpsDay, gpsMonth, gpsYear;

char strbuf[BUFSIZE];
char s1[BUF2SIZE];
char filename[13];
bool fileEnable;
bool runMode;
bool logFileOpened;

void setup() {

  pinMode(LED_PIN_NO, OUTPUT) ;      // LEDに接続
  pinMode(SW_PIN_NO, INPUT_PULLUP ) ; // SW に接続し内部プルアップに設定

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial Ready");

  // initialize OLED Display M096P4BL
//  Wire.begin();
//  Wire.setClock(400000L);
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

  // initialize Software Serial and GPS
  gps.begin(9600); // ソフトウェアシリアルの初期化
  // configure output of GM - 8013T
  configure_GP8013T();
//  Serial.println("GPS ready");
  oled.print("GPS ready");

  delay(3000);
  
// initialize SD card
  filename[0]='\0';
  Serial.println("Initializing SD card...");
  oled.setCursor(0,0);
  oled.clear();
  oled.print("Init SD card");
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (SD.begin(SD_CHIP_SELECT)) {
    fileEnable = checkSDFile();
  } else {
//    Serial.println("SD begin failed.");
    oled.setCursor(0,6);
//    oled.clearToEOL();
    oled.println("SD begin failed.");
    fileEnable=false;
  }
  logFileOpened=false;
  delay(3000);

  oled.setCursor(0,0);
  oled.clearToEOL();

  runMode = 0;
}

void loop()
{
  int swStatus;

  swStatus = digitalRead(SW_PIN_NO);
  if (runMode == 1 && swStatus == LOW) {
    runMode = 0; // logging turn off
    Serial.println("logging off");
    digitalWrite(LED_PIN_NO, LOW);
    if (logFileOpened == true) {
      logFile.close();
      logFileOpened = false;
    }
  } else if (runMode == 0 && swStatus == HIGH) { // logging turn on
    runMode = 1;
    Serial.println("logging started...");
    if (logFileOpened == false) {
      if(!fileEnable) {
        fileEnable = checkSDFile();
      }
      if(fileEnable) {
        logFile = SD.open(filename, FILE_WRITE);
      }
      
      if(!logFile || !fileEnable) {
        Serial.println("Can't open logfile");
        oled.setCursor(0,6);
        oled.print("Can't open logfile");
      } else {
        logFileOpened=true;
        digitalWrite(LED_PIN_NO, HIGH);
      }
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
  char GPSStr[7];
  char localTime0[9], localDate0[9];
  int offset;

  if (gps.available()) {  // if recived serial signal
    recvStr();   // read serial data to string buffer
    if (logFileOpened == true) {
      logFile.print(strbuf);
    } else {
      Serial.print(strbuf);
    }

    // Display date/time/latitude/longitude
    offset = strip_NMEA(strbuf, 0, 1);
    //      strcpy(GPSStr,s1);
    if (strcmp(s1, "$GNRMC") == 0) { // if RMC line
      offset = strip_NMEA(strbuf, offset, 1); // utcTime
      strcpy(utcTime, s1);
      offset = strip_NMEA(strbuf, offset, 1); // status
      strcpy(statGPS, s1);
      offset = strip_NMEA(strbuf, offset, 1); // latitue
      strcpy(latitude, s1);
      offset = strip_NMEA(strbuf, offset, 1); // N/W
      strcpy(NS, s1);
      offset = strip_NMEA(strbuf, offset, 1); // longitude
      strcpy(longtude, s1);
      offset = strip_NMEA(strbuf, offset, 1); // W/E
      strcpy(WE, s1);
      offset = strip_NMEA(strbuf, offset, 3); // utcDate
      strcpy(utcDate, s1);

      gpsDate(utcDate);
      gpsTime(utcTime);
      UCTtoLT();
      sprintf(localDate0, "%02d/%02d/%02d", gpsYear,gpsMonth,gpsDay);
      sprintf(localTime0, "%02d:%02d:%02d", gpsHour,gpsMin,gpsSec);

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

      // for debug
//      Serial.print(' ');
//      Serial.print(gpsYear);
//     Serial.print(gpsMonth);
//      Serial.print(gpsDay);
//      Serial.print(' ');
//      Serial.print(gpsHour);
//      Serial.print(gpsMin);
//      Serial.print(gpsSec);

    /*
      Serial.print(utcTime);
      Serial.print(' ');
      Serial.println(localTime0);
      Serial.print(utcDate);
      Serial.print(' ');
      Serial.println(localDate0);
      */
            /*
            Serial.print("latitude: ");
            Serial.println(latitude);
            Serial.print("longitude: ");
            Serial.println(longtude);
            */    
    }
  }
}

// recieve string from GPS
void recvStr()
{
  int i = 0;
  char c;
  while (1) {
    if (gps.available()) {
      c = gps.read();
      strbuf[i] = c;
      if (c == '\n') break;
      i++;
    }
  }
  i++;
  strbuf[i] = '\0';  // \0: end of string
}

// get info from NMEA sentence
int strip_NMEA(const char *orig, int offset, int count)
{
  char *str0, *s0;
  int i, len;

  for (i = 0; i < count; i++) {
    str0 = orig + offset;
    s0 = strchr(str0, ',');
    len = strlen(str0) - strlen(s0);
    strncpy(s1, str0, len); // len < BUF2SIZE +1 assumed
    s1[len] = '\0';
    offset += strlen(s1) + 1;
  }

  // for debug
  /*
    Serial.print(orig);
    Serial.print(";");
    Serial.print(offset);
    Serial.print(";");
    Serial.println(s1);
  */

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
    Serial.print(i);
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
  Serial.print("Opening log file: ");
  Serial.println(filename);
  oled.println("Opening log file");

// open log file
  logFile = SD.open(filename, FILE_WRITE);
  if (!logFile) {
    Serial.print("Can't open log file: ");
    Serial.println(filename);
    oled.setCursor(0,6);
    oled.print("Can't open logfile");
    return false;
  }
//  Serial.print("Log file opened:");
//          Serial.println(filename);
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

// send NMEA command with checksum to gps (software serial)
void send_nmea_command(const char *p)
{
  uint8_t checksum = 0;
  gps.print('$');
  do {
    char c = *p++;
    if (c) {
      checksum ^= (uint8_t)c;
      gps.print(c);
    }
    else {
      break;
    }
  }
  while (1);
  gps.print('*');
  gps.println(checksum, HEX);
  //  gps.print("\n\r");
}
void send_PUBX_packet(const char *p)
{
  uint8_t checksum = 0;
  gps.print('$');
  do {
    char c = *p++;
    if (c) {
      checksum ^= (uint8_t)c;
      gps.print(c);
    }
    else {
      break;
    }
  }
  while (1);
  gps.print('*');
  gps.println(checksum, HEX);
}

// $GNRMC, $GNVTG, $GNGGA, $GPGSV, $GLGSV, $GNGLL, $GNGSA
void configure_GP8013T()
{
  send_PUBX_packet("PUBX,40,RMC,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,VTG,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GGA,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GSV,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GLL,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GSA,0,5,0,5,0,0");
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

int mysubstr(char *t, const char *s, int pos, int len )
{
    if( pos < 0 || len < 0 || len > strlen(s) )
        return -1;
    for( s += pos; *s != '\0' && len > 0; len-- )
        *t++ = *s++;
    *t = '\0';
    return 0;
}

bool setFileName()
{
  char GPSStr[7];
  char utcTime[10], utcDate[7];
  int offset;

  Serial.println("Seeking filename");
  if (gps.available()) {  // if recived serial signal
    recvStr();   // read serial data to string buffer
    offset = strip_NMEA(strbuf, 0, 1);
    if (strcmp(s1, "$GNRMC") == 0) { // if RMC line
      Serial.println(strbuf);
      offset = strip_NMEA(strbuf, offset, 1); // utcTime
      strcpy(utcTime, s1);
      offset = strip_NMEA(strbuf, offset, 8); // utcDate
      strcpy(utcDate, s1);
      //        Serial.println(utcDate);
      if (strlen(utcDate) == 6 ) { // if date utcDate is valid
        gpsDate(utcDate);
        gpsTime(utcTime);
        UCTtoLT();
        sprintf(filename, "GP%02d%02d%02d.txt", gpsYear,gpsMonth,gpsDay);
        return true;
      }
    }
  }
  return false;
}
