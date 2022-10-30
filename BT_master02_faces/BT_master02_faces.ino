#include <M5Stack.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#define KEYBOARD_I2C_ADDR 0X08
#define KEYBOARD_INT      5

String slave_name = "BT_Slave";   // スレーブ側の接続名
String master_name = "BT_master"; // マスターの接続名
// uint8_t address[6]  = {0x94, 0xB9, 0x7E, 0x8B, 0x4F, 0xAA};  // MACアドレスで接続の場合、スレーブ側のMACアドレスを設定
bool connected = 0;       // 接続状態格納用
int connect_count = 3;    // 再接続実行回数
String data = "";         // 受信データ格納用
int btn_pw = 0;           // 電源ボタン状態取得用

// 再起動（リスタート）処理
/*
void restart() {
  // 電源ボタン状態取得（1秒以下のONで「2」1秒以上で「1」すぐに「0」に戻る）
  btn_pw = M5.Axp.GetBtnPress();
  if (btn_pw == 2) {  // 電源ボタン短押し（1秒以下）なら
    ESP.restart();    // 再起動
  }
}
*/

void setup() {
    M5.begin();
    M5.Power.begin();
    Wire.begin();
    M5.Lcd.setTextFont(2);
    M5.Lcd.println("FACES Keyboard I2C Read Example");
    pinMode(KEYBOARD_INT, INPUT_PULLUP);

  // Bluetooth接続開始
  SerialBT.begin(master_name, true);  // マスターとして初期化。trueを書くとマスター、省略かfalseを指定でスレーブ
  M5.Lcd.print("BT Try!\n.");
  Serial.print("BT Try!\n.");
  // connect(address)は高速（最大10秒）、connect(slave_name)は低速（最大30秒）
  // slave_nameでの接続はMACアドレスを調べなくても良いが接続は遅い
  while (connected == 0) {    // connectedが0(未接続)なら接続実行を繰り返す
    if (connect_count != 0) { // 再接続実行回数カウントが0でなければ
      connected = SerialBT.connect(slave_name); // 接続実行（接続名で接続する場合）
      // connected = SerialBT.connect(address);   // 接続実行（MACアドレスで接続する場合）
      M5.Lcd.print(".");
      connect_count--;        // 再接続実行回数カウント -1
    } else {                  // 再接続実行回数カウントが0なら接続失敗
      M5.Lcd.setTextColor(RED, BLACK);
      M5.Lcd.print("\nFailed!");
      Serial.print("\nFailed!");
//      while (1) {restart();}  // 無限ループ(再起動待機)
    }
  // 接続確認
  M5.Lcd.setTextColor(WHITE, BLACK);
  if (connected) {                    // 接続成功なら
    M5.Lcd.setTextColor(CYAN, BLACK);
    M5.Lcd.println("\nConnected!");   // 「Connected!」表示
    Serial.println("\nConnected!");
  } else {                            // 接続失敗なら
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.println("\nFailed!!");     // 「Failed!!」表示
    Serial.println("\nFailed!!");
//    while (1) {restart();}            // 無限ループ(再起動待機)
  }
  delay(1000);                        // 接続結果確認画面表示時間
  // 電源ON時のシリアルデータが無くなるまで待つ
  while (Serial.available()) {data = Serial.read();}
  // LCD表示リセット
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.println(master_name);        // 接続名表示
}
}

void loop() {
    if (digitalRead(KEYBOARD_INT) == LOW) {
        Wire.requestFrom(KEYBOARD_I2C_ADDR, 1);  // request 1 byte from keyboard
        while (Wire.available()) {
            uint8_t key_val = Wire.read();  // receive a byte as character
            if (key_val != 0) {
                if (key_val >= 0x20 && key_val < 0x7F) {  // ASCII String
                    Serial.print((char)key_val);
                    M5.Lcd.print((char)key_val);
                    if(connected==1) {
                      SerialBT.print((char)key_val);
                    }
                } else {
                    Serial.printf("0x%02X ", key_val);
                    M5.Lcd.printf("0x%02X ", key_val);
                    if(connected==1) {
                      SerialBT.printf("0x%02X ", key_val);
                    }
                }
            }
        }
    }
      M5.update();  // 本体ボタン状態更新
  // ボタン操作処理（スレーブ側へデータ送信）※送信のみでよければコメント部を有効にして省エネ
  if (M5.BtnA.wasPressed()) {     // ボタンAが押されていれば
    // SerialBT.connect();        // 再接続 connect(name)で名前解決されたアドレス、または、connect(address)のMACアドレスで再接続
    if (connected == 1) {
      SerialBT.print("A ON!\r");  // 「A ON!」送信（「CR」区切り文字）
      // disconnect();            // 省エネのため接続を遮断
    }
  }
  if (M5.BtnB.wasPressed()) {     // ボタンBが押されていれば
    // SerialBT.connect();           // 再接続 connect(name)で名前解決されたアドレス、または、connect(address)のMACアドレスで再接続
    if (connected == 1) {
      SerialBT.print("B ON!\r");  // 「B ON!」送信（「CR」区切り文字）
      // disconnect();               // 省エネのため接続を遮断
    }
  }

}
