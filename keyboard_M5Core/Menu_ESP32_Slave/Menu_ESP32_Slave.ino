// taken from :http://vividhobby.blog.fc2.com/blog-entry-833.html

//=====================================================================
// LovyanGFXとSDのR/WをESP32に組み込む標準設定　　　　　　2021 - 02- 14
//=====================================================================
// M5 Menu Application と　ペアで使用するESP32用Application
//=====================================================================
//
//        M5の" M5_Menu_App09"とこのESP32の" ESP32_Lov_SD_Lib004 "で
//        通信とLCD表示成功
//  （操作ヒント）
//  ESp32のリセット後、直ちにM5のリセットを行うと、双方起動後にペアリングが行われる
//  LCD（LovyanGFX）が未実装でもSerialにて動作確認できる。
//======================================================================
//　（注意）　ボードマネージャは  ESP32_Dev_Module を使用の事。
//======================================================================
//
//
//
//
//
//
//======================================================================

//========================================================================
//  SPIのESP32PIN接続（＊印ピンは任意で設定可能）
//  　　　　　　　　　　VSPI　　　　　　HSPI
//                 SD Card     TFT_Display
//          CS      5             15
//          *RESET  33            32
//          *DC     27            33
//          MOSI    23            13
//          MISO    19            12
//          CLK     18            14
//
//
//========================================================================



//---------------------------------------------------------------
//  ディスプレイ関係の設定
//---------------------------------------------------------------
//--------------------------------------------
//  Colorの定義（共通）
//--------------------------------------------
#define Red       0xbb0000U
#define Orange    0xff4500U
#define MedViolet 0xc71585U
#define DarkBlue  0x008080U
#define ForestGr  0x006400U
#define Gold      0xffd700U
#define MidNiBlue 0x191970U
#define Purple    0x800080U
#define Maroon    0x800000U
#define LightBlue 0x20b2aaU
#define Lime      0x00ff00U
#define DarkOrk   0x9932ccU
#define LightCamel 0xf08080U
#define Green     0x00ff00U
#define White     0xffffffU
#define Black     0x000000U

#include <LovyanGFX.hpp>
struct LGFX_Config
{
  static constexpr spi_host_device_t spi_host = HSPI_HOST;//  HSPI_HOSTに変更
  static constexpr int dma_channel = 1;
  static constexpr int spi_sclk = 14;// 18 -> 14
  static constexpr int spi_mosi = 13;//23 -> 13
  static constexpr int spi_miso = 12;
  static constexpr int spi_dlen = 8;
};

static lgfx::LGFX_SPI<LGFX_Config> lcd;
static lgfx::Panel_ST7735S panel;
static uint32_t sec, psec;
static size_t fps = 0, frame_count = 0;
static uint32_t lcd_width ;
static uint32_t lcd_height;

//------------------------------
//  Bluetooth
//------------------------------
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//======================================================================
//  SetUp関数
//======================================================================


void setup(void)
{
  int i, j, k;//  Loop変数
  char RGB[4];//読み込んだカラーデータ用
  char Read_Data[4];//読み込んだその他データ用
  unsigned long Pos, ans;//R/W ポイント

  //----------------------------------------------------------------------
  //  ESP32起動確認
  //----------------------------------------------------------------------

  delay(1000);//  起動まで１秒待つ
  //----  Boot Up Check  ----
  pinMode(0, OUTPUT);// Monitor LED
  for ( i = 0 ; i < 5 ; i ++) {
    delay(200);
    digitalWrite(0, HIGH);//
    delay(100);
    digitalWrite(0, LOW);
  }


  //------------------------
  //  シリアルとBluetooth
  //------------------------
  Serial.begin(115200);
  SerialBT.begin("ESP32", false); //Bluetooth Drive name as Slave mode
  Serial.println("ESP32 Slave mode. you can pair it with bluetooth!");


  //---------------------------------------------------------------------
  //  TFT_LCD の設定
  //---------------------------------------------------------------------
  //---------------------------------------------
  //  描画CLKを40MHzに挙げてもFPSは殆ど変化なし
  //  100MHz 位に上げると画面にドットのゴミが表示される。
  //---------------------------------------------
  panel.freq_write = 40000000;// 20MHz -> 40 MHz
  panel.freq_fill  = 40000000;// 20MHz -> 40 MHz
  panel.freq_read  = 40000000;// 20MHz -> 40 MHz

  //---------------------------------------------
  panel.spi_mode = 0;
  panel.spi_mode_read = 0;
  panel.len_dummy_read_pixel = 8;
  panel.spi_read = true;
  panel.spi_3wire = false;
  panel.spi_cs = 15;// 14 -> 15
  panel.spi_dc = 33;// 27 -> 33
  panel.gpio_rst = 32;// 33 -> 32

  panel.invert = true;
  panel.reverse_invert = true;
  panel.rgb_order = false;
  panel.memory_width  = 128;
  panel.memory_height = 160;
  panel.panel_width  = 80;
  panel.panel_height = 160;
  panel.offset_x = 24;
  panel.offset_y = 0;
  panel.rotation = 0;
  panel.offset_rotation = 0;
  //---------------------
  //  Lovyan 初期化
  //---------------------
  lcd.setPanel(&panel);
  lcd.init();
  lcd_width = lcd.width();
  lcd_height = lcd.height();


  //------------------------
  //  ST7735ディスプレイ初期化
  //------------------------
  lcd.setRotation(3);// 回転方向を 0～3 の4方向から設定します。(4～7を使用すると上下反転になります。)
  lcd.setBrightness(255);// バックライトの輝度を 0～255 の範囲で設定します。
  lcd.setTextSize(1);// 文字サイズ指定（文字サイズ１はかなり小さい）
  lcd.setColorDepth(24);  // RGB888の24ビットに設定(表示される色数はパネル性能によりRGB666の18ビットになります)

  //-----------------------
  //  画面描画テスト
  //-----------------------
  lcd.fillScreen(0xffffffU);  // 白で塗り潰し
  delay(500);
  lcd.fillScreen(0xff0000U);  // 赤で塗り潰し
  delay(500);
  lcd.fillScreen(0x00ff00U);  // 緑で塗り潰し
  delay(500);
  lcd.fillScreen(0x0000ffU);  // 青で塗り潰し
  delay(500);
  lcd.fillScreen(0x000000U);  // 黒で塗り潰し
  lcd.setTextFont(2);//Fontのドット数指定
  lcd.setCursor(0, 0);// カーソルセット
  lcd.setTextColor(0xffffffU, 0x000000U);//文字白色と背景黒色セット
  //---------------------------
  // LovyanGFX 設定完了メッセージ
  //---------------------------
  lcd.clear();
  lcd.println("Ready for LovyanGFX");//入力文字の表示
  lcd.println("ESP32 to connect");
  lcd.println("M5 core 2 with M5");
  lcd.println("ESP32_Lov_SD_Lib004");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);// カーソルセット
  Serial.println("ESP32_Lov_SD_Lib004");
}

void loop(void)
{
  char DT;
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    DT = SerialBT.read();
    //Serial.write(SerialBT.read());
    Serial.write(DT);
    lcd.print(DT);
  }
  delay(10);

}
