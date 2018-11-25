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

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "U8glib.h"

#define SD_CHIP_SELECT 10

// initialize the library with the numbers of the interface pins
SoftwareSerial gps(8, 9); // RX, TX

// setup u8g object
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI

File logFile;

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
 
  // initialize Software Serial and GT-720F
  gps.begin(9600); // ソフトウェアシリアルの初期化

// configure output of GM-8013T

//  send_nmea_command("PSRF103,00,00,2,01"); // GGA
//  send_nmea_command("PSRF103,01,00,2,01"); // GLL
//  send_nmea_command("PSRF103,02,00,2,01"); // GSA
//  send_nmea_command("PSRF103,03,00,2,01"); // GSV
//  send_nmea_command("PSRF103,04,00,2,01"); // RMC
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
  char *latitude, *longtude;

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
    if (strcmp(strtok(str, ","), "$GNRMC") == 0) { //if RMC line
      strtok(NULL, ",");
      latitude = strtok(NULL, ","); //get latitude
      strtok(NULL, ",");
      longtude = strtok(NULL, ","); //get longitude
      strtok
 
      u8g.firstPage();
      do {
        u8g.setFont(u8g_font_unifont);
        u8g.drawStr(0, 18,latitude);
        u8g.drawStr(0, 36,longtude);
      } while(u8g.nextPage());

      Serial.println(latitude);
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
  gpsHour = int(UTC/10000);
  gpsMin = int(UTC%10000/100);
  gpsSec = UTC%100;
}

// Latitude/Longitude
void gpsLatLong(int lat1, int lat2, int long1, int long2)
{
  gpsLat = int(lat1/100) + (lat1%100)/60.0 + float(lat2)/10000.0/60.0;
  gpsLong = int(long1/100) + (long1%100)/60.0 + float(long2)/10000.0/60.0;
}

// Date ->(gpsYear,gpsMonth,gpsYear)
void gpsDate(long dateRead)
{
  gpsDay = int(dateRead/10000);
  gpsMonth = int(dateRead%10000/100);
  gpsYear = dateRead%100; //last 2 digits, e.g. 2013-> 13

}

