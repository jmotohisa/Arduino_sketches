/*
  taken from: https://www.youtube.com/watch?v=UAqyy7OqpZY&list=PLMpGx3eIpze4tHBUwbVqbXscz-KODs4GO&index=13
  
 */

#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Wiring
/*
ESP32 | MAX31885 peripheral
--------|---------
18      | 4 (SCK)
19      | 3 (MISO)
16      | 1 (SS)
GND     | 5 (GND)
3V3     | 6 (VCC)

ESP32 | MAX 6675
------|------
19 | MISO
16 | SS
18 | SCK
3V3 | VCC
GND | GND

ILI9341 module | ESPP32
-------|---------------
1 (VCC)    | 3V3
2 (GND)    | GND
3 (CS)     | 14
4 (RESET)  | 33
5 (D/C)    | 27
6 (SDI(MOSI)) | 23
7 (SCK)    | 18
8 (LED)    | 3V3 (with 100ohm)
9 (SDO(MISO) | 19
10 (T_CLK) | 18 SCLK
11 (T_CS)  | 5
12 (T_DIN) | 23 MOSI
13 (T_DO)  | 19 MISO
14 (T_IRQ) | NC(4)

R1 (CS)     | 17
R2 (MOSI)   | MOSI
R3 (MISO)   | MISO
R4(J4) (SCK)    | SCK

RTC DS1307 (via level converter) | ESP32
SCL | 22
SDA | 21

 */
#define TFT_CS 14
#define TFT_RST 33
#define TFT_DC 27
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_MISO 19

#define TOUCH_SCLK TFT_CLK
#define TOUCH_CS 5
#define TOUCH_DIN TFT_MOSI
#define TOUCH_DOUT TFT_MISO
#define TOUCH_IRQ 4

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC,TFT_RST);

// XPT2046_Touchscreen ts = XPT2046_Touchscreen(TOUCH_CS);
XPT2046_Touchscreen ts= XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);  // Param 2 - Touch IRQ Pin - interrupt enabled polling


// initializer
//
//

typedef struct BUTTON{
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t t;
  char *key;
} BUTTON;

BUTTON key_C = { 20, 20,45,45,"C"};
BUTTON key_D = { 70, 20,45,45,"D"};
BUTTON key_E = {120, 20,45,45,"E"};
BUTTON key_F = {170, 20,45,45,"F"};
BUTTON key_8 = { 20, 70,45,45,"8"};
BUTTON key_9 = { 70, 70,45,45,"9"};
BUTTON key_A = {120, 70,45,45,"A"};
BUTTON key_B = {170, 70,45,45,"B"};
BUTTON key_4 = { 20,120,45,45,"4"};
BUTTON key_5 = { 70,120,45,45,"5"};
BUTTON key_6 = {120,120,45,45,"6"};
BUTTON key_7 = {170,120,45,45,"7"};
BUTTON key_0 = { 20,170,45,45,"0"};
BUTTON key_1 = { 70,170,45,45,"1"};
BUTTON key_2 = {120,170,45,45,"2"};
BUTTON key_3 = {170,170,45,45,"3"};

BUTTON *key_array[16] = { &key_0, &key_1, &key_2, &key_3, &key_4, &key_5, &key_6, &key_7,
			  &key_8, &key_9, &key_A, &key_B, &key_C, &key_D, &key_E, &key_F};

float calibration_x1;
float calibration_y1;
float delta_x,delta_y;

void draw_button(BUTTON *button, uint16_t color) {
  tft.drawRect(button->x,button->y,button->w,button->t,color);
  tft.setCursor(button->x+15, button->y+12);
  tft.setTextColor(color);
//  char s[3] = {button->key,'\0'};
  char s[3];
  strcpy(s,button->key);
  Serial.println(s);
//  strcat(s,'\0');
//  Serial.println(s);
  tft.setTextSize(2);
  tft.print(s);
}

void draw_buttons() {
  int i;
  for(i=0;i<16;i++) {
    draw_button(key_array[i],ILI9341_WHITE);
  }
}

boolean is_in_area(BUTTON *button, TS_Point point) {
  return((convert_x(point.x) > button->x) && ((convert_x(point.x) <button->x +button->w))
	 &&(convert_y(point.y) > button->y) && ((convert_y(point.y) <button->y +button->t)));
}

boolean is_touch_button(BUTTON *button) {
  TS_Point point = ts.getPoint();
  boolean is_touch = false;
  if(is_in_area(button,point)) {
    while(ts.touched()) {
      point= ts.getPoint();
      if(is_in_area(button,point)){
	draw_button(button,ILI9341_RED);
	is_touch=true;
      } else {
	draw_button(button,ILI9341_WHITE);
	is_touch = false;
      }
    }
    draw_button(button,ILI9341_WHITE);
  }
  return(is_touch);

}

void printpoint_raw(TS_Point p){
    Serial.print("Pressure = ");
    Serial.print(p.z);
    Serial.print(", x = ");
    Serial.print(p.x);
    Serial.print(", y = ");
    Serial.print(p.y);
    delay(30);
    Serial.println();
}

void printpoint_calibed(TS_Point p){
    Serial.print("Pressure = ");
    Serial.print(p.z);
    Serial.print(", x = ");
    Serial.print(convert_x(p.x));
    Serial.print(", y = ");
    Serial.print(convert_y(p.y));
    delay(30);
    Serial.println();
}


void calibration (void) {
  drawCrossPoint(20, 20, ILI9341_RED);
  drawCrossPoint(220, 300, ILI9341_WHITE);

  uint8_t count = 0;
  float sum_x = 0.0;
  float sum_y = 0.0;
  while(true) {
    while(ts.touched() && count<10) {
      TS_Point point = ts.getPoint();
      printpoint_raw(point);
      sum_x += point.x;
      sum_y += point.y;
      count++;
    }
    if(count==10) break;
  }
  calibration_x1 = sum_x / 10.0;
  calibration_y1 = sum_y / 10.0;
  Serial.print("Calibration (x1,y1)=(");
  Serial.print(calibration_x1);
  Serial.print(",");
  Serial.print(calibration_y1);
  Serial.println(")");
  
  delay(1000);
  
  drawCrossPoint(20, 20, ILI9341_WHITE);
  drawCrossPoint(220, 300, ILI9341_RED);

  count = 0;
  sum_x = 0.0;
  sum_y = 0.0;
  while(true) {
    while(ts.touched() && count<10) {
      TS_Point point = ts.getPoint();
      printpoint_raw(point);
      sum_x += point.x;
      sum_y += point.y;
      count++;
    }
    if(count==10) break;
  }
  float calibration_x2 = sum_x / 10.0;
  float calibration_y2 = sum_y / 10.0;
  Serial.print("Calibration (x2,y2)=(");
  Serial.print(calibration_x2);
  Serial.print(",");
  Serial.print(calibration_y2);
  Serial.println(")");

  delta_x = 200.0/ (calibration_x2-calibration_x1);
  delta_y = 280.0/ (calibration_y2-calibration_y1);

  drawCrossPoint(20, 20, ILI9341_BLACK);
  drawCrossPoint(220, 300, ILI9341_BLACK);

  char s[64];
  Serial.print("Cal(");
  Serial.print(calibration_x1);
  Serial.print(",");
  Serial.print(calibration_y1);
  Serial.println(")");
  Serial.print("Delta(");
  Serial.print(delta_x*1000.0);
  Serial.print(",");
  Serial.print(delta_y*1000.0);
  Serial.println(") at x1000");
}

void drawCrossPoint(uint16_t x, uint16_t y,uint16_t color){
  tft.drawLine(x-5,y,x+5,y,color);
  tft.drawLine(x,y-5,x,y+5,color);
  tft.drawCircle(x,y,2,color);
}

float convert_x(float x0){
  return (20.0+(float)(x0-calibration_x1)*delta_x);
}

float convert_y(float y0){
  return (20.0+(float)(y0-calibration_y1)*delta_y);
}
  
void setup() {
  Serial.begin(115200);

  Serial.println("Init TFT and Touch...");
  
//  Wire.begin();

  ts.begin();
  ts.setRotation(2);

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);

  calibration();

  delay(1000);

  draw_buttons();
  
}

void loop() {
  char *key='\0';

  if(ts.touched()) {
    for(uint8_t i=0;i<16;i++) {
      if(is_touch_button(key_array[i])) key= key_array[i]->key;
    }
  }

  if(key != '\0') {
    Serial.print("Touch:");
    Serial.println(key);
  }
}
  /*  
  if (ts.touched()) {
    TS_point p = ts.getPoint();
    Serial.print("Pressure = ");
    Serial.print(p.z);
    Serial.print(", x = ");
    Serial.print(convert_x(p.x));
    Serial.print(", 7 = ");
    Serial.print(convert_y(p.y));
    delay(30);
    Serial.println();
  }
}
*/
