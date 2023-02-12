// keyboard_test1

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
  uint8_t status;
} BUTTON;

#define WAIT 0
#define INPKEY 1
BUTTON key0_0 = { 20, 20,45,45,"W",WAIT};
BUTTON key0_1= { 70, 20,45,45,"N",INPKEY};
BUTTON *key0_array[2] = { &key0_0, &key0_1};

#define KEYIN  0
#define KEYCLR 1
#define KINFIN 2
#define SKIP   3
BUTTON key1_7 = { 20, 20,45,45,"7",KEYIN};
BUTTON key1_8 = { 70, 20,45,45,"8",KEYIN};
BUTTON key1_9 = {120, 20,45,45,"9",KEYIN};
BUTTON key1_4 = { 20, 70,45,45,"4",KEYIN};
BUTTON key1_5 = { 70, 70,45,45,"5",KEYIN};
BUTTON key1_6 = {120, 70,45,45,"6",KEYIN};
BUTTON key1_1 = { 20,120,45,45,"1",KEYIN};
BUTTON key1_2 = { 70,120,45,45,"2",KEYIN};
BUTTON key1_3 = {120,120,45,45,"3",KEYIN};
BUTTON key1_0 = { 20,170,45,45,"0",KEYIN};
BUTTON key1_clr = { 70,170,45,45,"CL",KEYCLR};
BUTTON key1_ok1 = {120,170,45,45,"OK",KINFIN};

BUTTON *key1_array[12] = { &key1_0, &key1_1, &key1_2, &key1_3, &key1_4, &key1_5, &key1_6, &key1_7,
			   &key1_8, &key1_9, &key1_clr, &key1_ok1};

float calibration_x1;
float calibration_y1;
float delta_x,delta_y;
bool touch_pressed=false;
bool touch_released=false;
TS_Point g_point;

void draw_button(BUTTON *button, uint16_t color) {
  tft.drawRect(button->x,button->y,button->w,button->t,color);
  tft.setCursor(button->x+15, button->y+12);
  tft.setTextColor(color);
//  char s[3] = {button->key,'\0'};
  char s[3];
  strcpy(s,button->key);
//  Serial.println(s);
//  strcat(s,'\0');
//  Serial.println(s);
  tft.setTextSize(2);
  tft.print(s);
}

void draw_keys0() {
  int i;
  for(i=0;i<2;i++) {
    draw_button(key0_array[i],ILI9341_WHITE);
  }
}

void draw_keys1() {
  int i;
  for(i=0;i<12;i++) {
    draw_button(key1_array[i],ILI9341_WHITE);
  }
}

boolean is_in_area(BUTTON *button, TS_Point point) {
  return((convert_x(point.x) > button->x) && ((convert_x(point.x) <button->x +button->w))
	 &&(convert_y(point.y) > button->y) && ((convert_y(point.y) <button->y +button->t)));
}

boolean is_touch_button(BUTTON *button) {
  TS_Point point = g_point;
  boolean is_touch = false;
  if(is_in_area(button,point)) {
    while(ts.touched()) {
      point= ts.getPoint();
//      point= g_point;
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
  g_point=point;
  return(is_touch);

}

float convert_x(float x0){
  return (20.0+(float)(x0-calibration_x1)*delta_x);
}

float convert_y(float y0){
  return (20.0+(float)(y0-calibration_y1)*delta_y);
}

void printpoint_raw(TS_Point p){
    Serial.print("Pressure = ");
    Serial.print(p.z);
    Serial.print(", x = ");
    Serial.print(p.x);
    Serial.print(", y = ");
    Serial.println(p.y);
    delay(30);
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

  Serial.println("Calibration: touch red star:");
  
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


#define MAXLEN 256
 char strbuf[MAXLEN];

int check_key1(int val_old) {
  int status=SKIP;
  uint8_t bufptr=0;
  boolean loopstatus=true;
  char *key;
  int ikey;
  int val;
  
//  tft.fillScreen(ILI9341_BLACK);

  Serial.print("Keyboard:status ");
  Serial.println(status);
  
  do {
    if((ikey=touched2(key1_array,12)) >= 0) {
      key = key1_array[ikey]->key;
      status = key1_array[ikey]->status;
    }
    switch (status) {
    case KEYIN:
      tft.setCursor(bufptr*12,0);
      tft.print(key);
      strbuf[bufptr]=*key;
      bufptr++;
      status = SKIP;
      break;
    case KEYCLR:
      tft.fillRect(0,0,bufptr*12,14,ILI9341_BLACK);
      bufptr=0;
      status=SKIP;
      break;
    case KINFIN:
      strbuf[bufptr]='\0';
      bufptr++;
      loopstatus=false;
      break;
    case SKIP:
    default:
      break;
    }
  }
  while(loopstatus);
  
  if(bufptr<=1) {
    val = val_old;
  } else {
    val = atoi(strbuf);
  }
  return val;
}

int touched2(BUTTON **key_array,int narray)
{
  TS_Point point;
  int ikey;
  boolean valid=false;
  if(ts.touched()) {
    point = ts.getPoint();
      for(uint8_t i=0;i<narray;i++) {
  if(is_in_area(key_array[i],point)) {
    valid = true;
    ikey = i;
    draw_button(key_array[ikey],ILI9341_RED);
  }
      }
  } else {
    return -1;
  }

  while(ts.touched()) {
    point = ts.getPoint();
    if(!is_in_area(key_array[ikey],point)) {
      valid = false;
      draw_button(key_array[ikey],ILI9341_WHITE);
    }
  }

  draw_button(key_array[ikey],ILI9341_WHITE);
  if(valid)
    return ikey;
  else
    return -1;
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

  draw_keys0();
  
}

void loop() {
  char *key='\0';
  int status=WAIT;
  int num;
  int i;
  int ikey;

  if((ikey = touched2(key0_array,2))>=0) {
    key= key0_array[ikey]->key;
    status = key0_array[ikey]->status;
    Serial.println(status);
  }
  
  switch (status) {
  case WAIT:
//    Serial.println("Status: WAIT");
    break;
  case INPKEY:
    Serial.println("Status: INPKEY");
    delay(100);
    touch_pressed = false;
    draw_keys1();
    num = check_key1(203);
    Serial.println(num);
    tft.fillScreen(ILI9341_BLACK);
    draw_keys0();
    status = WAIT;
    break;        
  default:
    break;
  }
  delay(50);
}
