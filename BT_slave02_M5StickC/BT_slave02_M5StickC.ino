// Bluetrooth Slave: M5StickC
// Original is taken from https://logikara.blog/arduino_bluetooth/

// Bluetooth接続スレーブ側プログラム
#include <M5StickC.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// 変数宣言
String name = "BT_Slave"; // 接続名を設定
int btn_pw = 0;           // 電源ボタン状態取得用
String data = "";         // 受信データ格納用

// 初期設定 -----------------------------------------------
void setup() {
  M5.begin();             // 本体初期化
  Serial.begin(9600);     // シリアル通信初期化
  // 出力設定
  pinMode(10, OUTPUT);    //本体LED赤
  digitalWrite(10, HIGH); //本体LED初期値OFF（HIGH）
  // LCD表示設定
  M5.Lcd.setTextFont(2);
  M5.Lcd.println(name); // 接続名表示

  // Bluetooth接続開始
  SerialBT.begin(name); // 接続名を指定して初期化。第2引数にtrueを書くとマスター、省略かfalseでスレーブ
  // MACアドレスの取得と表示
  uint8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);  // MACアドレス取得
  M5.Lcd.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
  // 電源ON時のシリアルデータが無くなるまで待つ
  while (Serial.available()) {data = Serial.read();}
  M5.Lcd.setTextFont(2);  // テキストサイズ変更

  pinMode(GPIO_NUM_10, OUTPUT);
}
// メイン -------------------------------------------------
void loop() {
  M5.update();  // 本体のボタン状態更新
  // ボタン操作処理（マスター側へデータ送信）
  if (M5.BtnA.wasPressed()) {   // ボタンAが押されていれば
    SerialBT.print("A ON!\r");  // 「A ON!」送信（「CR」区切り文字）
  }
  if (M5.BtnB.wasPressed()) {   // ボタンBが押されていれば
    SerialBT.print("B ON!\r");  // 「B ON!」送信（「CR」区切り文字）
  }
  // データ受信時処理
  // スレーブからの受信データをLCDに表示
  if (SerialBT.available()) {               // Bluetoothデータ受信で
    data = SerialBT.readStringUntil('\r');  // 区切り文字「CR」の手前までデータ取得
    M5.Lcd.print("BT: ");                   // 受信データ液晶表示
    M5.Lcd.println(data);                   // 液晶表示は改行あり
    Serial.print(data);                     // シリアル出力は改行なし
  }
  // シリアル入力データ（「CR」付きで入力）をスレーブ側へ送信
  if (Serial.available()) {               // シリアルデータ受信で
    data = Serial.readStringUntil('\r');  // 「CR」の手前までデータ取得
    M5.Lcd.print("SR: ");                 // シリアルデータ液晶表示
    M5.Lcd.println(data);                 // 液晶表示は改行あり
    Serial.print(data);                   // シリアル出力は改行なし
    // Bluetoothデータ送信
    SerialBT.print(data + "\r"); // 区切り文字「CR」をつけてマスター側へ送信
  }
  // LED点灯処理
  if (data == "A ON!") {      // 受信データが「A ON!」なら
    digitalWrite(GPIO_NUM_10, LOW);    // 本体LED点灯
  }
  if (data == "B ON!") {      // 受信データが「B ON!」なら
    digitalWrite(GPIO_NUM_10, HIGH);   // 本体LED消灯
  }
  // 再起動（リスタート）処理
  if (btn_pw == 2) {          // 電源ボタン短押し（1秒以下）なら
    ESP.restart();
  }
  // 電源ボタン状態取得（1秒以下のONで「2」1秒以上で「1」すぐに「0」に戻る）
  btn_pw = M5.Axp.GetBtnPress();
  delay(20);  // 20ms遅延時間
}
