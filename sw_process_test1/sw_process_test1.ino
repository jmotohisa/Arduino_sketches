/* Test of sw_process.h 
 *  Using Sunhayato AS-E401 LCD/Switch shield
*/

/* Sunhayato AS-E401 LCD/Switch shield default setting 
 *  
 *  LCD       :  Arudino
 *  DB7       : 2
 *  DB6       : 3 
 *  DB5       : 4
 *  DB5       : 5
 *  E         : 11
 *  RS        : 12
 *  backlight : 5V
 *  SW1       : 6
 *  SW2       : 7
 */

// include the library code:
#include <LiquidCrystal.h>
#include <MsTimer2.h>
#include "sw_process.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define SW1 6
#define SW2 7
#define MAX_COUNT 100
sw_process(SW1,SW1)
sw_process(SW2,SW2)

int count=0;
int waittime=10;
int en_quick=0;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  sw_SW1_process(!digitalRead(SW1));
  sw_SW2_process(!digitalRead(SW2));

  lcd.setCursor(0,0);
  lcd.print(count);
//  delay(waittime);
}

void sw_SW1_push()
{  
  countup();
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW1 pushed");
}

void sw_SW1_release()
{
  MsTimer2::stop();
  en_quick=0;
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW1 released");
}

void sw_SW1_hold()
{
  MsTimer2::set(100,enable_quick);
  MsTimer2::start();
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW1 hold");
}

void sw_SW1_wpush()
{
}

void sw_SW1_holdon()
{
  if(en_quick) {
    en_quick=0;
    countup();
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW1 holdon");
  }
}

void enable_quick()
{
  en_quick=1;
}

void sw_SW2_push()
{
  countdown();
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW2 pushed");
}

void sw_SW2_release()
{
  MsTimer2::stop();
  en_quick=0;
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW2 released");
}

void sw_SW2_hold()
{
  MsTimer2::set(100,enable_quick);
  MsTimer2::start();  
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW2 hold");
}

void sw_SW2_wpush()
{
}

void sw_SW2_holdon()
{
  if(en_quick) {
    en_quick=0;
    countdown();
  }
  lcd_clearline(1);
  lcd.setCursor(0,1);
  lcd.print("SW2 holdon");
}

void lcd_clearline(int line)
{
  lcd.setCursor(0,line);
  lcd.print("                ");
}

void countup()
{
  if(count!=MAX_COUNT) { 
    count++;
  }
}

void countdown()
{
  if(count!=0) {
    count--;
  }
}
