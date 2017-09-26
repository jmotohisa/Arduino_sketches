// my7segshieldtest1.c

#include <SevenSeg.h>

SevenSeg disp(2,3,4,5,6,7,8);

const int numOfDigits=8;
int digitPins[numOfDigits]={A5,A4,A3,A2,A1,A0,9,11};

void setup() {

  // 7seg
  disp.setDigitPins(numOfDigits, digitPins);
  disp.setTimer(1);
  disp.startTimer();
}

void loop() {
  // put your main code here, to run repeatedly:
  disp.write(12);
  delay(500);
  disp.write(12.3);
  delay(500);
  disp.write(-12.3);
  delay(500);
  disp.write(-12);
  delay(500);
}

ISR(TIMER1_COMPA_vect)
{
  disp.interruptAction();
}

