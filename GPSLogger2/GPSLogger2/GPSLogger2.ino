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
bool fileOpened = false;

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
  Serial.println("Ready");

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

// configure output of GT-720F
    // 0xA0, 0xA1, 0x00, 0x09, 0x08, followed by GGA/GSA/GSV/GLL/RMC/VTG/ZDA intervals, attributes and checksum, CR, LF

    // GGA/GSA/GSV/GLL/RMC messages, 1second
    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x0D, 0x0A};
     // GGA and RMC message only, 1second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x0A};
     // GGA and RMC message only, 2second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x0A};
     // GGA/GSV/RMC message, 2second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0A, 0x0D, 0x0A};
     // GGA/GSV/RMC message only, 5second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x0A};
     // GGA/GSA/GSV/GLL/RMC messages, 5second
//    byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x0D, 0x0D, 0x0A};
   gps.write(message,sizeof(message));
  Serial.println("GPS ready");

  // initialize SD card
  Serial.print("Initializing SD card...");
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (SD.begin(SD_CHIP_SELECT)) {
    fileOpened = checkSDFile();
  }

  delay(3000);
}

void loop()
{
  char str[90]; // string buffer
  char *latitude, *longtude;

  if (fileOpened && !logFile)
  {
    logFile = SD.open(filename, FILE_WRITE);
  }

  if (gps.available()) {  // if recived serial signal
    recvStr(str);   // read serial data to string buffer
    Serial.println(str);
    if (logFile) {
      logFile.println(str);
    }
    if (strcmp(strtok(str, ","), "$GPRMC") == 0) { //if RMC line
      strtok(NULL, ",");
      strtok(NULL, ",");
      latitude = strtok(NULL, ","); //get latitude
      strtok(NULL, ",");
      longtude = strtok(NULL, ","); //get longtude
 
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

