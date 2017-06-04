/* ADC and LCD to read and display analog out of Varian Multiguage

  The circuit: NOKIA LCD5110 and Akizukidenshi LTC2450 ADC

 * LCD5110
//      SCK  - Pin 8
//      MOSI - Pin 9
//      DC   - Pin 10
//      RST  - Pin 11
//      CS   - Pin 12


 * Akizukidenshi LTC2450 DIP module
 * 1 (+V) to VDD
 * 2 (Vin) to vin (with 1kohm/1kohm divider)
 * 3 (GND) :GND 
 * 4 h(SPI CS) to digital pin 10
 * 5 (SPI SDO) to digital pin 12
 * 6 (SPI SCL) to digital pin 13

 // Read analog pin A5
 Read LTC2450 16bit ADC input
 and convert to pressure (in Torr) for Varian multiguage
*/

// include the library code:
#include <LCD5110_Basic.h>
//#include <SPI.h>

LCD5110 myGLCD(8,9,10,11,12);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

int sensorPin = A5; // input pin
float vout = 0;
unsigned char m_data; //MSB byte
unsigned char l_data; //LSB byte
const int CS = 10;

void setup() {

// LCD
  myGLCD.InitLCD();

// ADC
  analogReference(DEFAULT);
//  pinMode(CS,OUTPUT);
//  SPI.begin();
//  SPI.setBitOrder(MSBFIRST);
//  SPI.setDataMode(SPI_MODE3);
//  SPI.setClockDivider(SPI_CLOCK_DIV16);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int dig;
  float exponent;
  
  vout = analogRead(sensorPin)*4.883*0.001*2;
//  digitalWrite(CS,LOW);
//  m_data = SPI.transfer(0x00);
//  l_data = SPI.transfer(0x00);
//  digitalWrite(CS,HIGH);
// /*  vout = (m_data*2^8+l_data)/2^16*2*5; */
//  vout=m_data*0.039062+l_data*1.5259e-4;
  Serial.println(vout);

  myGLCD.setFont(SmallFont);
  myGLCD.print("output=",LEFT,0);
  myGLCD.printNumF(vout,LEFT,0);
  myGLCD.print("V",LEFT,0);

  dig=(int) vout;
  exponent = pow(10,1-vout+dig);
  myGLCD.setFont(BigNumbers);
  myGLCD.printNumF(exponent,LEFT,24);
  myGLCD.print("x10^-",LEFT,0);
  myGLCD.printNumI(dig,LEFT,0);
  myGLCD.print("Torr",LEFT,0);
  delay(500);
}
