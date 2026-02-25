// http://www.geocities.co.jp/arduino_diecimila/use/

/*
  GPS NMEA data reader by Kimio Kosaka 
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

void setup(){
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  // initialize the serial communications:
  Serial.begin(9600);
  lcd.clear();
}

void loop()
{
  char str[90]; // string buffer
  char *latitude,*longtude;
  if (Serial.available()) {  // if recived serial signal
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
  }
}

void recvStr(char *buf)
{
  int i = 0;
  char c;
  while (1) {
    if (Serial.available()) {
      c = Serial.read();
      buf[i] = c;
      if (c == '\n') break; 
      i++;
    }
  }
  buf[i] = '\0';  // \0: end of string
}
