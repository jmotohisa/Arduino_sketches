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

/* Arduino-GPS (Software Serial)
 *  RX: 8
 *   TX: 9
*/
 
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

#define SD_CHIP_SELECT 10

// initialize the library with the numbers of the interface pins
/* lcd(RS, E, DB4, DB5, DB6, DB7) */
LiquidCrystal lcd(7, 6,  5, 4, 3, 2);
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
        lcd.print(fileTmp);
        if (!SD.exists(fileTmp)) {
            logFile = SD.open(filename, FILE_WRITE);
            Serial.println(fileTmp);
            if (logFile) {
                lcd.print("OK");
                Serial.println("Log file opend");
                strcpy(fileTmp,filename);
                return true;
            }
            lcd.setCursor(0,1
            );
            lcd.print("Can't open file");
            Serial.println("Can't open logfile");
            break;
        }
    }
    return false;
}

void setup() {
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Ready");

// Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ready");

  gps.begin(4800); // ソフトウェアシリアルの初期化

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
  if(!fileOpened) {
    logFile = SD.open(filename,FILE_WRITE);
    fileOpened=true;
  }
  if (gps.available()) {  // if recived serial signal
    recvStr(str);   // read serial data to string buffer
    if(strcmp(strtok(str,","),"$GPRMC")==0){ //if RMC line
       strtok(NULL,",");
       strtok(NULL,",");
       latitude=strtok(NULL,","); //get latitude
       strtok(NULL,",");
       longtude=strtok(NULL,","); //get longtude
       lcd.setCursor(0,0);
       lcd.print(latitude); // show latitude
       lcd.setCursor(0,1);
       lcd.print(longtude); // show longtude
    }
    Serial.println(str);
    if(fileOpened) {
     logFile.println(str);
    logFile.close();
    fileOpened=false;
    }
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
