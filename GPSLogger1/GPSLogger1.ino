// GPS Logger version 1

/* note 
Arudino-SDcard
 The circuit: 
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module. ->10
*/

/* Arduino-LCD
  The circuit:
 * LCD RS pin to digital pin 12 -> 7
 * LCD Enable pin to digital pin 11 -> 6
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

/* Arduino-GPS GT-720F (with Software Serial)
 *  RX: 8 - 6
 *  TX: 9 - 5
 *  5: input (TTL) - 9 (TX) - 6: output (TTL) - 8 (RX)
*/
 
//#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

#define SD_CHIP_SELECT 10

// initialize the library with the numbers of the interface pins
/* lcd(RS, E, DB4, DB5, DB6, DB7) */
//LiquidCrystal lcd(7, 6,  5, 4, 3, 2);
SoftwareSerial gps(8, 9); // RX, TX

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
                Serial.println("Log file opend");
                strcpy(filename,fileTmp);
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
  // set up the LCD's number of rows and columns: 
//  lcd.begin(16, 2);
//  lcd.clear();
//  lcd.print("Ready");

// Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ready");

  gps.begin(9600); // ソフトウェアシリアルの初期化
  // taken from http://arms22.blog91.fc2.com/blog-entry-299.html
//  send_pmtk_packet("PMTK220,1000");
//  send_pmtk_packet("PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
//  send_pmtk_packet("PSRF103,2,0,0,1");
//  send_pmtk_packet("PSRF103,3,0,0,1");
//  send_pmtk_packet("PSRF103,5,0,0,1");

//  byte message[]={0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x0A};
//  gps.write(message,sizeof(message)); // RMC and GGA message only
//  Serial.println("GPS ready");
  
  pinMode(SD_CHIP_SELECT,OUTPUT);
  if(SD.begin(SD_CHIP_SELECT)) {
    fileOpened = checkSDFile();
  }
  delay(3000);
}

void loop()
{
  char str[90]; // string buffer
  char *latitude,*longtude;

  if(fileOpened && !logFile)
  {
     logFile=SD.open(filename,FILE_WRITE);
  }
  
  if (gps.available()) {  // if recived serial signal
    recvStr(str);   // read serial data to string buffer
  //  Serial.println(str);
    if(logFile) {
        logFile.println(str);
    }
    if(strcmp(strtok(str,","),"$GPRMC")==0){ //if RMC line
       strtok(NULL,",");
       strtok(NULL,",");
       latitude=strtok(NULL,","); //get latitude
       strtok(NULL,",");
       longtude=strtok(NULL,","); //get longtude
//       lcd.setCursor(0,0);
//       lcd.print(latitude); // show latitude
//       lcd.setCursor(0,1);
//       lcd.print(longtude); // show longtude
//       Serial.println(latitude);
//       Serial.println(longtude);
    }
//    Serial.println(str);
  }
  if(logFile) {
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

void send_pmtk_packet(char *p)
{
  uint8_t checksum = 0;
  gps.print('$');
  do {
    char c = *p++;
    if(c){
      checksum ^= (uint8_t)c;
      gps.print(c);
    }
    else{
      break;
    }
  }
  while(1);
  gps.print('*');
  gps.println(checksum,HEX);
}

