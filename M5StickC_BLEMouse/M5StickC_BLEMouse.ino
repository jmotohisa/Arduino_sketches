/*
 * M5StkickC BLE mouse
 * 
 * Taken from 
 * https://qiita.com/norikawamura/items/162ccba193fe0c964237
*/


#include <BleConnectionStatus.h>
#include <BleMouse.h>

#include <M5StickC.h>

BleMouse bleMouse;
signed char mouse_x = 0;
signed char mouse_y = 0;
float mouse_min = 200;

float accX = 0;
float accY = 0;
float accZ = 0;

float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;

float temp = 0;
void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(40, 0);
  M5.Lcd.println("BLE Mouse");
  //M5.Lcd.setCursor(0, 15);
  //M5.Lcd.println("  X       Y       Z");
  M5.MPU6886.Init();
  bleMouse.begin();
  while(bleMouse.isConnected() == false) {
    delay(100);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //M5.MPU6886.getGyroData(&gyroX,&gyroY,&gyroZ);
  M5.MPU6886.getAccelData(&accX,&accY,&accZ);
  //M5.MPU6886.getTempData(&temp);

  //M5.Lcd.setCursor(0, 30);
  //M5.Lcd.printf("%.2f   %.2f   %.2f      ", gyroX, gyroY,gyroZ);
  //M5.Lcd.setCursor(140, 30);
  //M5.Lcd.print("o/s");
  //M5.Lcd.setCursor(0, 45);
  //M5.Lcd.printf("%.2f   %.2f   %.2f      ",accX * 1000,accY * 1000, accZ * 1000);
  //M5.Lcd.setCursor(140, 45);
  //M5.Lcd.print("mg");
  //M5.Lcd.setCursor(0, 60);
  //M5.Lcd.printf("Temperature : %.2f C", temp);

  M5.update();
  if(M5.BtnA.isPressed()){
    if(M5.BtnB.isPressed()){
      bleMouse.press(MOUSE_LEFT | MOUSE_RIGHT);
    }else{
      bleMouse.release(MOUSE_RIGHT);
      bleMouse.press(MOUSE_LEFT);
    }
  }else{
    if(M5.BtnB.isPressed()){
      bleMouse.release(MOUSE_LEFT);
      bleMouse.press(MOUSE_RIGHT);
    }else{
      bleMouse.release(MOUSE_LEFT | MOUSE_RIGHT);
    } 
  }

  mouse_x = 0;
  mouse_y = 0;
  if(accX * 1000 > mouse_min){
    mouse_x = -1 * (accX * 1000) / mouse_min;
  }
  if(accX * 1000 < mouse_min * -1){
    mouse_x = -1 * (accX * 1000) / mouse_min;
  }
  if(accY * 1000 > mouse_min){
    mouse_y = 1 * (accY * 1000) / mouse_min;
  }
  if(accY * 1000 < mouse_min * -1){
    mouse_y = 1 * (accY * 1000) / mouse_min;
  }

  bleMouse.move(mouse_x, mouse_y);

  //delay(50);
}
