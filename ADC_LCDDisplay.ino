/* ADC and LCD

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
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

 // Read analog pin A5
 // and convert to pressure (in Torr) for Varian multiguage
*/

// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// int sensorPin = A5; // input pin
float vout = 0;
unsigned char m_data; //MSB byte
unsigned char l_data; //LSB byte
const int CS = 10;

void setup() {
  // put your setup code here, to run once:
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
//  lcd.print("ADC value from A5");

// ADC
//  analogReference(DEFAULT);
  pinMode(CS,OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
}

void loop() {
  // put your main code here, to run repeatedly:
  int dig;
  float exponent;
//  vout = analogRead(sensorPin)*4.883*0.001*2
  digitalWrite(CS,LOW);
  m_data = SPI.transfer(0x00);
  l_data = SPI.transfer(0x00);
  digitalWrite(CS,HIGH);
  vout=(m_data*256+l_data)*4.883*0.001*2;
  
  lcd.setCursor(0,0);
  lcd.print("output=");
  lcd.setCursor(7,0);
  lcd.print(vout);
  lcd.setCursor(12,0);
  lcd.print("V");
  lcd.setCursor(0,1);
  dig=(int) vout;
  exponent = pow(10,1-vout+dig);
  lcd.print(exponent);
  lcd.setCursor(5,1);
  lcd.print("x10^-");
  lcd.setCursor(10,1);
  lcd.print(dig);
  lcd.setCursor(12,1);
  lcd.print("Torr");
  delay(500);
}
