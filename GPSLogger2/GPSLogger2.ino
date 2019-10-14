// GPS Logger version 2

/* with
  GT-720F
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

/* Arduino-GPS GT-720F (with Software Serial)
    RX: 8 - 6 (TTL output)
    TX: 9 - 5 (TTL input)
*/

/* Arudio - M096PBL
    A5 - SCL
    A4 - SDA
  v*/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "SSD1306AsciiAvrI2c.h"

#define SD_CHIP_SELECT 10
#define TimeZone (9)
#define SW_PIN_NO 6
#define LED_PIN_NO 7
#define BUFSIZE 100
#define BUF2SIZE 12

// initialize the library with the numbers of the interface pins
SoftwareSerial gps(8, 9); // RX, TX

// setup u8g object
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

//SSD1306AsciiWire oled;
SSD1306AsciiAvrI2c oled;

// NMEA string to analyze latitude, logitude, date and time
#define NMEA_RMC_STRING "$GPRMC"

File logFile;

char filename[13];

int gpsHour, gpsMin, gpsSec;
int gpsDay, gpsMonth, gpsYear;

char strbuf[BUFSIZE];
char s1[BUF2SIZE];
char substr[7];
bool fileEnable;
bool runMode;
bool logFileOpened;
bool fileOpened = false;

void setup() {
  // Open serial communications and wait for port to open:
//  Serial.begin(9600);
//  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
//  }
//  Serial.println("Ready");

  pinMode(LED_PIN_NO, OUTPUT) ;      // LEDに接続
  pinMode(SW_PIN_NO, INPUT_PULLUP ) ; // SW に接続し内部プルアップに設定

  // initialize OLED Display M096P4BL
  // flip screen, if required
  // u8g.setRot180();
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);
  oled.setFont(X11fixed7x14);
//  oled.setFont(System5x7);
  oled.clear();
  oled.setCursor(0,0);

 
  // initialize Software Serial and GT-720F
  gps.begin(9600); // ソフトウェアシリアルの初期化

// configure output of GT-720F
  // 0xA0, 0xA1, 0x00, 0x09, 0x08, followed by GGA/GSA/GSV/GLL/RMC/VTG/ZDA intervals, attributes and checksum, CR, LF

  // GGA/GSA/GSV/GLL/RMC messages, 1second
  byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x0D, 0x0A};
  // GGA and RMC message only, 1second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x0A};
  // GGA and RMC message only, 2second
  //  byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x0A};
  // GGA/GSV/RMC message, 2second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0A, 0x0D, 0x0A};
  // GGA/GSV/RMC message only, 5second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x0A};
     // GGA/GSA/GSV/GLL/RMC messages, 5second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x0D, 0x0D, 0x0A};
   gps.write(message,sizeof(message));
//   Serial.println("GPS ready");

  // initialize SD card
  filename[0]='\0';
  oled.setCursor(0,0);
  oled.clear();
  oled.print("Init SD card");
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (SD.begin(SD_CHIP_SELECT)) {
    fileEnable = checkSDFile();
  } else {
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
    digitalWrite(LED_PIN_NO, LOW);
    if (logFileOpened == true) {
      logFile.close();
      logFileOpened = false;
    }
  } else if (runMode == 0 && swStatus == HIGH) { // logging turn on
    runMode = 1;
    if (logFileOpened == false) {
      if(!fileEnable) {
        fileEnable = checkSDFile();
      }
      if(fileEnable) {
        logFile = SD.open(filename, FILE_WRITE);
      }
      
      if(!logFile || !fileEnable) {
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
//    digitalWrite(LED_PIN_NO,HIGH);
    recvStr();   // read serial data to string buffer
//    Serial.println(strbuf);
    if (logFileOpened == true) {
      logFile.print(strbuf);
      logFile.flush();
    } else {
  }
//    digitalWrite(LED_PIN_NO,LOW);

    // Display date/time/latitude/longitude
    offset = strip_NMEA(strbuf, 0, 1);
    //      strcpy(GPSStr,s1);
    if (strcmp(s1, NMEA_RMC_STRING) == 0) { // if RMC line
//      Serial.println(strbuf);
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
      oled.setCursor(70,6);
      oled.clearToEOL();
      oled.print(utcDate);

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

    }
  }
}

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
  strbuf[i] = '\0';  // \0: end of string
  
  oled.setCursor(0,6);
  strncpy(substr,strbuf,6);
  substr[6]='\0';
  oled.print(substr);
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

  return offset;
}

bool checkSDFile()
{
  // open log file
  for (unsigned int index = 0; index < 65535; index++) {
    char fileTmp[13];
    sprintf(fileTmp, "GPS%05d.TXT", index);
    //        lcd.print(fileTmp);
    if (!SD.exists(fileTmp)) {
      logFile = SD.open(fileTmp, FILE_WRITE);
//      Serial.println(fileTmp);
      if (logFile) {
        //                lcd.print("OK");
//        Serial.println("Log file opened");
        strcpy(filename, fileTmp);
        logFile.close();
        return true;
      }
      //            lcd.setCursor(0,1);
      //            lcd.print("Can't open file");
//      Serial.println("Can't open logfile");
      break;
    }
  }
  return false;
}

// send NMEA command with checksum to gps (hardware serial)
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

  if (gps.available()) {  // if recived serial signal
//    digitalWrite(LED_PIN_NO,HIGH);
    recvStr();   // read serial data to string buffer
    offset = strip_NMEA(strbuf, 0, 1);
    if (strcmp(s1, NMEA_RMC_STRING) == 0) { // if RMC line
      offset = strip_NMEA(strbuf, offset, 1); // utcTime
      strcpy(utcTime, s1);
      offset = strip_NMEA(strbuf, offset, 8); // utcDate
      strcpy(utcDate, s1);
      if (strlen(utcDate) == 6 ) { // if date utcDate is valid
        gpsDate(utcDate);
        gpsTime(utcTime);
        UCTtoLT();
        sprintf(filename, "GP%02d%02d%02d.txt", gpsYear,gpsMonth,gpsDay);
        return true;
      }
    }
//    digitalWrite(LED_PIN_NO,LOW);
  }
  return false;
}
