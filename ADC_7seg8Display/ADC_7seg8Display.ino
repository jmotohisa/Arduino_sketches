/* ADC and 7sev Display
 *  
  The circuit: Akizukidenshi LTC2450 ADC and my-8digit 7seg sheild

  
 * Akizukidenshi LTC2450 DIP module
 * 1 (+V) to VDD
 * 2 (Vin) to vin (with 1kohm/1kohm divider)
 * 3 (GND) :GND 
 * 4 h(SPI CS) to digital pin 10
 * 5 (SPI SDO) to digital pin 12
 * 6 (SPI SCL) to digital pin 13

 * my 8digit 7seg sheild

*/
#include <SevenSeg.h>
#include <SPI.h>

SevenSeg disp(2,3,4,5,6,7,8);

float vout = 0;
unsigned char m_data; //MSB byte
unsigned char l_data; //LSB byte
const int CS = 10;

const int numOfDigits=8;
int digitPins[numOfDigits]={A5,A4,A3,A2,A1,A0,9,11};

void setup() {
  
  disp.setDigitPins(numOfDigits, digitPins);
  
// ADC
//  analogReference(DEFAULT);
  pinMode(CS,OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);

// Debug
  Serial.begin(9600);

}  

void loop() {
 // put your main code here, to run repeatedly:
 
 int dig;
 float exponent;
  
//  vout = analogRead(sensorPin)*4.883*0.001*2;
 digitalWrite(CS,LOW);
 m_data = SPI.transfer(0x00);
 l_data = SPI.transfer(0x00);
 digitalWrite(CS,HIGH);
/*  vout = (m_data*2^8+l_data)/2^16*2*5; */
 vout=m_data*0.039062+l_data*1.5259e-4;
//  Serial.println(vout);
 dig=(int) vout;
 exponent = pow(10,1-vout+dig);

// Debug
 Serial.println(vout);

 disp.write(vout);
 delay(500);
}
