// BlueTooth serial and LabView

// https://forums.ni.com/t5/北東北-LabVIEWユーザー会/ESP32-BluetoothSerial-and-LabVIEW/td-p/4033489?profile.language=ja


#include <M5StickC.h>
#include "BluetoothSerial.h"
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
BluetoothSerial SerialBT;
void setup() {
Serial.begin(115200);
Serial.println("M5_Acc.ino");
 
SerialBT.begin("ESP32");
 
M5.begin();
M5.IMU.Init();
M5.Lcd.setRotation(3);
M5.Lcd.fillScreen(BLACK);
M5.Lcd.setTextSize(2);
}
void loop() {
M5.IMU.getAccelData(&accX,&accY,&accZ);
//Serial.printf("%7.4f, %7.4f, %7.4f\n", accX, accY, accZ);
SerialBT.printf("%7.4f, %7.4f, %7.4f\n", accX, accY, accZ);
M5.Lcd.setCursor(20, 5);
M5.Lcd.printf("X: %7.4f", accX);
M5.Lcd.setCursor(20, 30);
M5.Lcd.printf("Y: %7.4f", accY);
M5.Lcd.setCursor(20, 55);
M5.Lcd.printf("Z: %7.4f", accZ);
delay(10);
}
 
