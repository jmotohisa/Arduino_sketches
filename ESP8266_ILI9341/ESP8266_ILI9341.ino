/*
  Control test of ILI9341 with ESP8266 using SPI
  taken from https://qiita.com/toyoshim/items/84c026e97f6be200cb19
*/

#include <ESP8266WiFi.h>

/*
 * ILI9341   - ESP8266
 * VCC
 * GND
 * CS        - GPIO15 (CS, D8)
 * RESET     - GPIO5  (D3)
 * D/C       - GPIO6  (D4)
 * SDI(MOSI) - MOSI, GPIO13, D7
 * SCK       - SCLK (GPIO14, D5)
 * LED       - 3.3 V (with 100ohm)
 * SDD(MISO) - N/C (MISO GPIO12, D6)
*/

/*
#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
*/
#include <SPI.h>

// 後ろから前からどうぞ、ではなく縦画面と横画面の切り替え
#define PORTRAIT

#define CMD_SLEEP_OUT 0x11
#define CMD_DISPLAY_ON 0x29
#define CMD_COLUMN_ADDRESS_SET 0x2a
#define CMD_PAGE_ADDRESS_SET 0x2b
#define CMD_MEMORY_WRITE 0x2c
#define CMD_MEMORY_ACCESS_CONTROL 0x36
#define CMD_COLMOD 0x3a

#define MAC_PORTRAIT 0xe8
#define MAC_LANDSCAPE 0x48
#define COLMOD_16BIT 0x55
#define COLMOD_18BIT 0x66

#if defined(PORTRAIT)
# define MAC_CONFIG MAC_PORTRAIT
# define WIDTH 320
# define HEIGHT 240
#else
# define MAC_CONFIG MAC_LANDSCAPE
# define WIDTH 240
# define HEIGHT 320
#endif

enum {
  TFT_CS = D8,
  TFT_RESET = D3,
  TFT_RS = D4 // RS = DC
};

// for Arduino
// この3本はGPIO使ってます
// SPIのMOSIとSCKはArduino Pro Miniだと15、17
/*
enum {
  TFT_CS = 2,
  TFT_RESET = 3,
  TFT_RS = 4
};
*/


void write_command(uint8_t c) {
  digitalWrite(TFT_RS, LOW);
  SPI.transfer(c);
}

void write_data(uint8_t d) {
  digitalWrite(TFT_RS, HIGH);
  SPI.transfer(d);
}

void write_data16(uint16_t d) {
  digitalWrite(TFT_RS, HIGH);
  SPI.transfer16(d);
}

void lcd_init() {
  // リセット後の20ms待ちはデータシートが要求してます
  digitalWrite(TFT_RESET, LOW);
  delay(20);
  digitalWrite(TFT_RESET, HIGH);
  delay(20);

  // 液晶しか繋いでないのでチップセレクトは常時選択
  digitalWrite(TFT_CS, LOW);

  // メモリの読み出し方向の設定で縦画面、横画面が作れる
  write_command(CMD_MEMORY_ACCESS_CONTROL);
  write_data(MAC_CONFIG);

  // デフォルトの18bitモードより16bitモードの方が速くて楽
  write_command(CMD_COLMOD);
  write_data(COLMOD_16BIT);

  // スリープ解除後の60ms待ちはデータシートが要求してます
  write_command(CMD_SLEEP_OUT);
  delay(60);
  write_command(CMD_DISPLAY_ON);
}

void set_update_rect(uint16_t sx, uint16_t ex, uint16_t sy, uint16_t ey) {
  // VRAM内の書き込み矩形領域を設定するとCMD_MEMORY_WRITEに続くデータがその領域内に書かれる
  write_command(CMD_COLUMN_ADDRESS_SET);
  write_data16(sx);
  write_data16(ex);
  write_command(CMD_PAGE_ADDRESS_SET);
  write_data16(sy);
  write_data16(ey);
  write_command(CMD_MEMORY_WRITE);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
//  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_RESET, OUTPUT);
  pinMode(TFT_RS, OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  lcd_init();
  
  Serial.begin(115200);
  delay(500);
  Serial.println("LCD ready");
  Serial.print("MOSI (GPOI13):");
  Serial.println(D7);
  Serial.print("SCLK (GPIO14):");
  Serial.println(D5);
  Serial.print("MISO (GPOI12):");
  Serial.println(D6); 
  Serial.print("TFT_CS (GPIO15): ");
  Serial.println(D8);
  Serial.print("TFT_RESET (GPIO5): ");
  Serial.println(D3);
  Serial.print("TFT_RS (GPIO4): ");
  Serial.println(D4);
/*
 * CS        - GPIO15 (CS, D8)
 * RESET     - GPIO5  (D3)
 * D/C       - GPIO6  (D4)
 * SDI(MOSI) - MOSI, GPIO13, D7
 * SCK       - SCLK (GPIO14, D5)
 * LED       - 3.3 V (with 100ohm)
 * SDD(MISO) - N/C (MISO GPIO12, D6)
*/
}

void loop() {
  uint16_t c = 0;
  set_update_rect(0, WIDTH, 0, HEIGHT);

  // やってる事は以下と同義
//   for (uint16_t y = 0; y < HEIGHT; ++y)
//     for (uint16_t x = 0; x < WIDTH; ++x)
  //     write_data16(RRRRRGGGGGGBBBBB);
  for (uint16_t i = 0; i < 65536; ++i) 
  {
//      Serial.println(c,HEX);
      write_data16(c);
      write_data16(c);
      write_data16(c);
      write_data16(c);
      write_data16(c);
      write_data16(c);
      write_data16(c);
      write_data16(c++);
      yield();
  }
}
