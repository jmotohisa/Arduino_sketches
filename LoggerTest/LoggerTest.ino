// LoggerTest

/* read analog value and log into SD card */

/*
Arudino-SDcard
 The circuit: 
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module. ->10
 * 
 *  // Read analog pin A5
*/

#include <SPI.h>
#include <SD.h>

#define SD_CHIP_SELECT 10

char filename[13];
bool fileOpened = false;

File logFile;
int sensorPin = A5; // input pin
float vout = 0;

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
                Serial.println("Log file opend");
                strcpy(filename,fileTmp);
                logFile.close();
                return true;
            }
            Serial.println("Can't open logfile");
            break;
        }
    }
    return false;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ready");
  
  analogReference(DEFAULT);

  pinMode(SD_CHIP_SELECT,OUTPUT);
  if(SD.begin(SD_CHIP_SELECT)) {
    fileOpened = checkSDFile();
  }
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  String dataString="";
//  char str[90]; // string buffer
//  char *latitude,*longtude;
//  if(!fileOpened) {
//    logFile = SD.open(filename,FILE_WRITE);
//    fileOpened=true;
//  }
  
  vout = analogRead(sensorPin)*4.883*0.001*2;
  dataString=String(vout);

  logFile=SD.open(filename,FILE_WRITE);
  if(logFile) {
    logFile.println(dataString);
    logFile.close();
    Serial.println(dataString);
  }

  fileOpened=false;
  delay(500);
}
