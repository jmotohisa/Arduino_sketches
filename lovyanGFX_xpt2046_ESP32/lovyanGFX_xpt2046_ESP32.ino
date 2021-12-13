// v1.0.0 を有効にします(v0からの移行期間の特別措置です。これを書かない場合は旧v0系で動作します。)
#define LGFX_USE_V1

#define USE_TOUCH

//#include <ESP32.h>
/*
#define TFT_CS 14
#define TFT_RST 33
#define TFT_DC 27
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_MISO 19

#define TOUCH_SCLK 18
#define TOUCH_CS 5
#define TOUCH_DIN 23
#define TOUCH_DOUT 19
#define TOUCH_IRQ 22
*/
// 準備したクラスのインスタンスを作成します。

#ifdef USE_TOUCH
  #include "LGFX_ESP32_SPI_ILI9341_XPT2046.hpp"
  LGFX_ESP32_SPI_ILI9341_XPT2046 display;
#else
  #include "LGFX_ESP32_SPI_ILI9341.hpp"
  LGFX_ESP32_SPI_ILI9341 display;
#endif  

void setup(void)
{
/*
  // turn off wifi
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
*/
  Serial.begin(115200);

  // SPIバスとパネルの初期化を実行すると使用可能になります。
  display.init();

  display.setTextSize((std::max(display.width(), display.height()) + 255) >> 8);
  
  // タッチが使用可能な場合のキャリブレーションを行います。（省略可）
  if (display.touch())
  {
    if (display.width() < display.height()) display.setRotation(display.getRotation() ^ 1);
    
    // 画面に案内文章を描画します。
    display.setTextDatum(textdatum_t::middle_center);
    display.drawString("touch the arrow marker.", display.width()>>1, display.height() >> 1);
    display.setTextDatum(textdatum_t::top_left);

    // タッチを使用する場合、キャリブレーションを行います。画面の四隅に表示される矢印の先端を順にタッチしてください。
    std::uint16_t fg = TFT_WHITE;
    std::uint16_t bg = TFT_BLACK;
    if (display.isEPD()) std::swap(fg, bg);
    display.calibrateTouch(nullptr, fg, bg, std::max(display.width(), display.height()) >> 3);
  }

  display.fillScreen(TFT_BLACK);
}

uint32_t count = ~0;
void loop(void)
{
  display.startWrite();
  display.setRotation(++count & 7);
  display.setColorDepth((count & 8) ? 16 : 24);

  display.setTextColor(TFT_WHITE);
  display.drawNumber(display.getRotation(), 16, 0);

  display.setTextColor(0xFF0000U);
  display.drawString("R", 30, 16);
  display.setTextColor(0x00FF00U);
  display.drawString("G", 40, 16);
  display.setTextColor(0x0000FFU);
  display.drawString("B", 50, 16);

  display.drawRect(30,30,display.width()-60,display.height()-60,count*7);
  display.drawFastHLine(0, 0, 10);

  display.endWrite();

#ifdef USE_TOUCH
  int32_t x, y;
  if (display.getTouch(&x, &y)) {
    Serial.print(x,y);
    display.fillRect(x-2, y-2, 5, 5, count*7);
  }
#endif
  delay(100);
}
