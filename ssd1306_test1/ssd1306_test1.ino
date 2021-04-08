// taken from https://www.mgo-tec.com/blog-entry-ssd1306-revalidation-i2c-esp32-esp8266.html/2

// SSD1306 128x64


#include <Wire.h>
  
const uint8_t ADDRES_OLED =  0x3D;
const int SDA_OLED =  5;
const int SCL_OLED =  4;
const uint32_t Frequensy_OLED = 400000; //Max=400kHz
  
uint8_t DotB1[8]={
  0b11111111,
  0b00000111,
  0b11111111,
  0b00000011,
  0b00000101,
  0b00001001,
  0b00010001,
  0b00100001
};
  
void setup() {
  Serial.begin(115200);
  
  SSD1306_Init(); //OLED ssd1306 初期化
  delay(10);
  Clear_Display_All();
  Display_Pic();
}
  
void loop() {
    
}
  
void SSD1306_Init(){
  Wire.begin();
  Wire.setClock(Frequensy_OLED);
  delay(100);
    
  Wire.beginTransmission(ADDRES_OLED);//※このバイトも含め、以後、合計32byteまで送信できる
    Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
      Wire.write(0xAE); //display off
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0xA8); //Set Multiplex Ratio  0xA8, 0x3F
        Wire.write(0b00111111); //64MUX
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)    
      Wire.write(0xD3); //Set Display Offset 0xD3, 0x00
        Wire.write(0x00);
    Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
      Wire.write(0x40); //Set Display Start Line 0x40
    Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
      Wire.write(0xA0); //Set Segment re-map 0xA0/0xA1
    Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
      Wire.write(0xC0); //Set COM Output Scan Direction 0xC0,/0xC8
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0xDA); //Set COM Pins hardware configuration 0xDA, 0x02
        Wire.write(0b00010010);
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0x81); //Set Contrast Control 0x81, default=0x7F
        Wire.write(255); //0-255
    Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
      Wire.write(0xA4); //Disable Entire Display On
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0xA6); //Set Normal Display 0xA6, Inverse display 0xA7
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0xD5); //Set Display Clock Divide Ratio/Oscillator Frequency 0xD5, 0x80
        Wire.write(0b10000000);
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0x20); //Set Memory Addressing Mode
        Wire.write(0x10); //Page addressing mode
  Wire.endTransmission();
  Wire.beginTransmission(ADDRES_OLED);
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0x22); //Set Page Address
        Wire.write(0); //Start page set
        Wire.write(7); //End page set
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0x21); //set Column Address
        Wire.write(0); //Column Start Address
        Wire.write(127); //Column Stop Address
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0x8D); //Set Enable charge pump regulator 0x8D, 0x14
        Wire.write(0x14);
    Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
      Wire.write(0xAF); //Display On 0xAF
  Wire.endTransmission();
}
//*******************************************
void Display_Pic(){
  Wire.beginTransmission(ADDRES_OLED);
    Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
      Wire.write(0xB0); //set page start address←垂直開始ページはここで決める(B0～B7)
    Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
      Wire.write(0x21); //set Column Address
        Wire.write(0); //Column Start Address←水平開始位置はここで決める(0～127)
        Wire.write(127); //Column Stop Address　画面をフルに使う
  Wire.endTransmission();
  
  Wire.beginTransmission(ADDRES_OLED); //この後は Max 31byteまで
    Wire.write(0b01000000); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
    for(int i=0; i<8; i++){
      Wire.write(DotB1[i]); //SSD1306のGDRAM にデータ書き込み
    }
  Wire.endTransmission(); //これが送信されて初めてディスプレイに表示される
}
  
//**************************************************
void Clear_Display_All(){
  uint8_t i, j, k;
  
  for(i = 0; i < 8; i++){//Page(0-7)
    Wire.beginTransmission(ADDRES_OLED);
      Wire.write(0b10000000); //control byte, Co bit = 1, D/C# = 0 (command)
        Wire.write(0xB0 | i); //set page start address(B0～B7)
      Wire.write(0b00000000); //control byte, Co bit = 0, D/C# = 0 (command)
        Wire.write(0x21); //set Column Address
          Wire.write(0); //Column Start Address(0-127)
          Wire.write(127); //Column Stop Address(0-127)
    Wire.endTransmission();
  
    for(j = 0; j < 16; j++){//column = 8byte x 16
      Wire.beginTransmission(ADDRES_OLED);
        Wire.write(0b01000000); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
        for(k = 0; k < 8; k++){ //continue to 31byte
          Wire.write(0x00);
        }
      Wire.endTransmission();
    }
  }
}
