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
  char strbuf[90];
  char latitude[11], longtude[12];
  char NS[2], WE[2];
  //  String utcTime, utcDate;
  String s, s0;
  int pos;
  String statGPS, localDate, localTime;
  char localDate0[7], localTime0[7];
  int swStatus;
  String utcTime, utcDate;

  if (gps.available()) {  // if recived serial signal
    recvStr(strbuf);   // read serial data to string buffer
    s = String(strbuf);
    if (logFile) {
      logFile.println(s);
      logFile.flush();
    }
    pos = s.indexOf("$GNRMC");
    Serial.println(s);
    if (pos == 0) { // if RMC line
      pos = strip_NMEA(s, &utcTime, pos, 1); // 1: utcTime
      pos = strip_NMEA(s, &statGPS,  pos, 1); // 2: status
      pos = strip_NMEA(s, &s0, pos, 1); // 3: latitude
      s0.toCharArray(latitude, 11);
      pos = strip_NMEA(s, &s0,     pos, 1); // 4: North/South
      s0.toCharArray(NS, 2);
      pos = strip_NMEA(s, &s0,     pos, 1); // 5: longitue
      s0.toCharArray(longtude, 12);
      pos = strip_NMEA(s, &s0,     pos, 1); // 6: West/East
      s0.toCharArray(WE, 2);
      pos = strip_NMEA(s, &utcDate, pos, 3); // 2: utcDate

      u8g.firstPage();
      do {
        u8g.setFont(u8g_font_unifont);
        utcTime.toCharArray(localTime0, 7);
        utcDate.toCharArray(localDate0, 7);

        byte ofs = u8g.drawStr(0, 18, localTime0);
        u8g.drawStr(ofs + 10, 18, localDate0);

        if (statGPS.equals("A")) {
          u8g.drawStr(0, 36, NS);
          u8g.drawStr(10, 36, latitude);
          u8g.drawStr(0, 48, WE);
          u8g.drawStr(10, 48, longtude);
        }
      } while (u8g.nextPage());

      Serial.print(localTime0);
      Serial.print(' ');
      Serial.println(localDate0);
      Serial.print("latitude: ");
      Serial.println(latitude);
      Serial.print("longitude: ");
      Serial.println(longtude);

    }
  }
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

int strip_NMEA(String s, String *message, int pos, int count)
{
  int pos0, pos1, i = 0;
  pos0 = pos;
  Serial.println(s);
  pos1 = s.indexOf(",", pos0);
  while (i < count) {
    pos0 = pos1 + 1;
    pos1 = s.indexOf(",", pos0);
    i += 1;
  }
  *message = s.substring(pos0, pos1);

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

  return pos1;
}

void send_PUBX_packet(char *p)
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

void configure_GP8013T(int rate)
{
  send_PUBX_packet("PUBX,40,GGA,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,VTG,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GSV,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,RMC,0,1,0,1,0,0");
  send_PUBX_packet("PUBX,40,GSV,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GLL,0,5,0,5,0,0");
  send_PUBX_packet("PUBX,40,GSA,0,5,0,5,0,0");
}

