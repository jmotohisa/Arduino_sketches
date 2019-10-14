// GPS Logger version 3

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
    5: input (TTL) - 9 (TX) - 6: output (TTL) - 8 (RX)
*/

/* Arudio - M096PBL
    A5 - SCL
    A4 - SDA
  */

#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
//#include "U8glib.h"

#define SD_CHIP_SELECT 10
#define BUFSIZE 70
#define BUF2SIZE 12

// initialize the library with the numbers of the interface pins
SoftwareSerial gps(8, 9); // RX, TX

// setup u8g object
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

File logFile;

char filename[13];
bool fileEnable = false;
char s1[BUF2SIZE];

void recvStr(char *);
int strip_NMEA(const char *, int , int );
  
bool checkSDFile()
{
  // open log file
  for (unsigned int index = 0; index < 65535; index++) {
    char fileTmp[13];
    sprintf(fileTmp, "GPS%05d.TXT", index);
    Serial.println(index);
    if (!SD.exists(fileTmp)) {
      logFile = SD.open(fileTmp, FILE_WRITE);
      Serial.println(fileTmp);
      if (logFile) {
        Serial.println("Log file opened");
        strcpy(filename, fileTmp);
        //        logFile.close();
        return true;
      }
      Serial.println("Can't open log file");
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
  Wire.begin();
  Wire.setClock(400000L);
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(X11fixed7x14);
  oled.clear();

  // initialize Software Serial and GT-720F
  gps.begin(9600); // ソフトウェアシリアルの初期化
  // configure output of GM-8013T
  // configure_GP8013T();
  Serial.println("GPS ready");

  // initialize SD card
  Serial.print("Initializing SD card...");
  oled.print("Initializing SD card...");
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (SD.begin(SD_CHIP_SELECT)) {
    Serial.println("searching file");
    fileEnable = checkSDFile();
  } else {
    Serial.println("SD not enable");
    oled.setCursor(0,6);
    oled.print("SD not enable");
  }

  oled.setCursor(0,0);
  oled.clearToEOL();
  delay(3000);
}

void loop()
{
  char strbuf[BUFSIZE];
  char utcTime[10], utcDate[7];
  char latitude[11], longtude[12];
  char NS[2], WE[2];
  char statGPS[2];
  char GPSStr[7];
  char localTime0[10], localDate0[7];
  int offset;

  if (gps.available()) {  // if recived serial signal
    recvStr(strbuf);   // read serial data to string buffer
    Serial.println(strbuf);

    if (logFile) {
      logFile.print(strbuf);
      logFile.flush();
    }

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

      strcpy(localTime0, utcTime);
      strcpy(localDate0, utcDate);
        
      oled.setCursor(0,0);
      oled.print(localTime0);
      oled.setCursor(48,0);
      oled.print(localDate0);

        if (strchr(statGPS, 'A')) {
          oled.setCursor(0,2);
          oled.print(NS);
          oled.print(latitude);
          oled.setCursor(0,4);
          oled.print(WE);
          oled.print(longtude);
        }

      Serial.print(localTime0);
      Serial.print(' ');
      Serial.println(localDate0);
      Serial.print("latitude: ");
      Serial.println(latitude);
      Serial.print("longitude: ");
      Serial.println(longtude);

    }
  }
  logFile.close();
}

// recive string from GPS
void recvStr(char *strbuf)
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
}

int strip_NMEA(const char *orig, int offset, int count)
{
//  char str0[BUFSIZE], *s0;
  char *str0,*s0;
  int i, len;

  for (i = 0; i < count; i++) {
//    strcpy(str0, orig + offset);
    str0=orig+offset;
    s0 = strchr(str0, ',');
    len = strlen(str0) - strlen(s0);
    strncpy(s1, str0, len); // len < BUF2SIZE +1 assumed
    s1[len] = '\0';
    offset += strlen(s1) + 1;
  }
  /* printf("%s ; %d ; %s\n",s1,len,str); */
  //  strcpy(str, s1);

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

void configure_GP8013T()
{
  send_PUBX_packet("PUBX,40,GGA,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,VTG,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GSV,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,RMC,0,1,0,1,0,0");
  send_PUBX_packet("PUBX,40,GSV,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GLL,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GSA,0,5,0,5,0,0");
}
