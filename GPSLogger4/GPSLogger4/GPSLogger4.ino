// GPS Logger version 2

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
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "U8glib.h"

#define SD_CHIP_SELECT 10
#define TimeZone (9)

// initialize the library with the numbers of the interface pins
SoftwareSerial gps(8, 9); // RX, TX

// setup u8g object
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI

File logFile;

int gpsHour, gpsMin, gpsSec;
int gpsDay, gpsMonth, gpsYear;


char filename[13];
bool fileEnable = false;

bool checkSDFile()
{
  // open log file
  for (unsigned int index = 0; index < 65535; index++) {
    char fileTmp[13];
    sprintf(fileTmp, "GPS%05d.TXT", index);
    //        lcd.print(fileTmp);
    if (!SD.exists(fileTmp)) {
      logFile = SD.open(fileTmp, FILE_WRITE);
      Serial.println(fileTmp);
      if (logFile) {
        //                lcd.print("OK");
        Serial.println("Log file opened");
        strcpy(filename, fileTmp);
        logFile.close();
        return true;
      }
      //            lcd.setCursor(0,1);
      //            lcd.print("Can't open file");
      Serial.println("Can't open logfile");
      break;
    }
  }
  return false;
}

bool configure_GP8013T(int timeout) {

  return true;
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial Ready");

  // initialize OLED Display M096P4BL
  // flip screen, if required
  // u8g.setRot180();

  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
  u8g.setFont(u8g_font_helvR10r);  //1.6KByte/17x22

  // initialize Software Serial and GPS
  gps.begin(9600); // ソフトウェアシリアルの初期化
  // configure output of GM-8013T
  configure_GP8013T(60);
  Serial.println("GPS ready");

  // initialize SD card
  Serial.print("Initializing SD card...");
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (SD.begin(SD_CHIP_SELECT)) {
    fileEnable = checkSDFile();
  }

  delay(3000);
}

void loop()
{
  char str[90]; // string buffer
  char latitude[12], longtude[11];
  char *stat, *NS, *WE;
  String utcTime, utcDate;
  String s, s0;
  int pos;
  String localDate,localTime;
  char localDate0[7],localTime0[7];
  if (fileEnable)
  {
    logFile = SD.open(filename, FILE_WRITE);
  }

  if (gps.available()) {  // if recived serial signal
    recvStr(str);   // read serial data to string buffer
    Serial.println(str);
    if (logFile) {
      logFile.println(str);
    }
    s = str;
    pos = s.indexOf("$GNRMC");
    if (pos >= 0) { // if RMC line
//      strip_NMEA(s, &utcDate, pos, 9); strip date
//      Serial.print("utcDate:");
//      Serial.println(utcDate);
      pos = strip_NMEA(s, &utcTime, pos, 1); // 1: utcTime
      pos = strip_NMEA(s, &s0,     pos, 1); // 2: status
      s0.toCharArray(stat, 1);
      pos = strip_NMEA(s, &s0, pos, 1); // 3: latitude
      s0.toCharArray(latitude, 11);
      pos = strip_NMEA(s, &s0,     pos, 1); // 4: North/South
      s0.toCharArray(NS, 1);
      pos = strip_NMEA(s, &s0,     pos, 1); // 5: longitue
      s0.toCharArray(longtude, 10);
      pos = strip_NMEA(s, &s0,     pos, 1); // 6: West/East
      s0.toCharArray(WE, 1);
      pos = strip_NMEA(s, &utcDate, pos, 3); // 2: utcDate

//      long l_utcTime = utcTime.toInt();
//      long l_utcDate = utcDate.toInt();
//      gpsDate(l_utcTime);
//      gpsTime(l_utcDate);
//      UCTtoLT();

      u8g.firstPage();
      do {
        u8g.setFont(u8g_font_unifont);
        localTime = utcTime;
        localDate = utcDate;
//        localTime=String(gpsHour)+String(gpsMin)+String(gpsSec);
//        localDate=String(gpsYear)+String(gpsMonth)+String(gpsMonth);
        utcTime.toCharArray(localTime0,7);
        utcDate.toCharArray(localDate0,7);

        byte ofs = u8g.drawStr(0, 18, localTime0);        
        u8g.drawStr(ofs + 10, 18, localDate0);

        if (*stat == 'A') {
          u8g.drawStr(0, 36, NS);
          u8g.drawStr(10, 36, latitude);
          u8g.drawStr(0, 48, WE);
          u8g.drawStr(10, 48, longtude);
        }
      } while (u8g.nextPage());

      Serial.print(utcTime);
      Serial.print(' ');
      Serial.println(utcDate);
      Serial.print("latitude: ");
      Serial.println(latitude);
      Serial.print("longitude: ");
      Serial.println(longtude);
    }
    //    Serial.println(str);
  }
  if (logFile) {
    logFile.close();
  }
}

void recvStr(char *buf)
{
  int i = 0;
  char c;
  while (1) {
    if (gps.available()) {
      c = gps.read();
      buf[i] = c;
      if (c == '\n') break;
      i++;
    }
  }
  buf[i] = '\0';  // \0: end of string
}

int strip_NMEA(String s, String *message, int pos, int count)
{
  int pos0, pos1, pos0new, i = 0;
  pos0 = pos;
  pos1 = s.indexOf(",", pos0);
  while (i < count) {
    pos0 = pos1 + 1;
    pos1 = s.indexOf(",", pos0);
    i += 1;
  }
  *message = s.substring(pos0, pos1);  
/*  
   Serial.print(s);
    Serial.print(";");
    Serial.print(pos);
    Serial.print (",");
    Serial.print(count);
    Serial.print(";");
    Serial.print(pos0);
    Serial.print(",");
    Serial.print(pos1);
    Serial.print(",");
    Serial.println(*message);
*/
  return pos1;
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
  gps.print("\n\r");
}
// UTC -> (gpsHour, gpsMin, gpsSec)
void gpsTime(long UTC)
{
  gpsHour = int(UTC / 10000);
  gpsMin = int(UTC % 10000 / 100);
  gpsSec = UTC % 100;
}
/*
  // Latitude/Longitude
  void gpsLatLong(int lat1, int lat2, int long1, int long2)
  {
  gpsLat = int(lat1/100) + (lat1%100)/60.0 + float(lat2)/10000.0/60.0;
  gpsLong = int(long1/100) + (long1%100)/60.0 + float(long2)/10000.0/60.0;
  }
*/
// Date ->(gpsYear,gpsMonth,gpsYear)
void gpsDate(long dateRead)
{
  gpsDay = int(dateRead / 10000);
  gpsMonth = int(dateRead % 10000 / 100);
  gpsYear = dateRead % 100; //last 2 digits, e.g. 2013-> 13
}

void UCTtoLT()
{
  int leap=(gpsYear%4)*4/gpsYear - (gpsYear%100)*100/gpsYear + (gpsYear%400)*400/gpsYear ;
  int DaysAMonth[12] = {31, 28+leap, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  gpsHour +=TimeZone;
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
