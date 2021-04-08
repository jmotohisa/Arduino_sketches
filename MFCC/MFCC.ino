// Mass flow controller

// (1) DAC/Digital potentiometer
// I2C, MCP4726 (Akizuki module), address 0x60 (A0=0) or 0x60 (A0=1)


// Display
// buttons

#include <Wire.h>
#include <SPI.h>
#include <MsTimer2.h>

#define UP_BUTTON 7
#define DOWN_BUTTON 6
#define OPEN_BUTTON 5
#define CLOSE_BUTTON 4
#define FULLSCALE 10
#define DAC_ADDRESS 0x60

double reading;
double setting;

void setup() {
  // put your setup code here, to run once:

  //ADC
  Wire.beginTransmission

// pin mode
  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(OPEN_BUTTON, INPUT);
  pinMode(CLOSE_BUTTON, INPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}


if(prevStatus==false && buttonStatus) {
  pressedTime=millis();
  buttonLongPressed=false;
}

buttonStatus = buttonStatus & buttonPressed
if(buttonStatus && millis()-pressedTime>=1000) {
  buttonLongPressed = true;
} else {
  buttonLongPressed = false;
}
prevStatus = buttonStatus
void updateDisplay()
{

  
}

void setDAC(const unsigned int val)
{
  Wire.beginTransmission(DAC_ADDRESS);
  Wire.write(val,2);
  Wire.endTransmission();
}
