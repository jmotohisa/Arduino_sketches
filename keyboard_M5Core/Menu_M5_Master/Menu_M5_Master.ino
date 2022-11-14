// taken from :http://vividhobby.blog.fc2.com/blog-entry-833.html

//===========================================================================
//  BlueTooth Menu Select Application  (Master) for M5STack  Core2 2021-02-05
//===========================================================================
//  メニュー形式でデータを選択しBTで送信する。（このM5側をMasterとして設定する）
//
//-------------------------------------------------------------------------------------
//       ボードマネージャは、M5_Stack_Core_ESP32で実行。M5Core2だと起動しないことがある。
//       2021-02-11 追記
//-------------------------------------------------------------------------------------
//
//
//
//=======================================================================



//--------  Bluetooth接続の注意点  ----------------------------
//
//  （Master側）
//
//  MACaddとnameには接続相手（Slave側）のアドレスと名前を記載する
//  serialBT.begin("Masterの自分の名前", true); //masterとして始める
//  char *pin = "1234"とし、
//  connectで
//  connected = SerialBT.connect(address);接続相手のSlaveのアドレス指定
//
//  （Slave側）
//
//  begin("Slave自分の名前", false);// Slaveの指定
//
//  （接続起動順番）
//
//  Master側の電源投入後、BT始動後にSlave側の電源起動、BT始動する。
//  この順番でないとペアリングができないことがある
//
//
//-----------------------------------------------------------





//=============================================
//  M5STack用アプリをM5Core2でテスト
//  M5Stack.hの代わりにM5Core2.hを定義し、
//  LovyanGFX.hを定義してディスプレイ表示を変更
//  先にM5Core2.hを定義しないとERRORになるので注意
//=============================================

#include <M5Core2.h>//  M5Stack用BaseLibrary
#include <LovyanGFX.hpp>//  グラフィック用ライブラリ

static LGFX lcd;                 // LGFXのインスタンスを作成。
static LGFX_Sprite sprite(&lcd); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。


//------------------------------------------------------------------------
//  BlueTooth 関連の初期設定（本機M5StackをMasterとしESP32をSlaveとして接続する
//------------------------------------------------------------------------
#include "BluetoothSerial.h"// BlueTooth
BluetoothSerial SerialBT;

//--------------------------------------------
//  Colorの定義（共通）
//--------------------------------------------

//----  メニューバーの色定義  ------------------
//  Mfc, Mbc = Main Tytle F-color / B-color
//  Sfc, Sbc = Sub Tytle F-color / B-color
//  Rfc, Rbc = Rev Tytle F-color / B-color
//-------------------------------------------

uint32_t Mfc, Mbc, Sfc, Sbc, Rfc, Rbc;

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


//--------------------------------------------
//  Menuのレイアウト定義
//--------------------------------------------
//  ボタンの位置と大きさの設定
//  Y+38 で二倍の高さのボタン
//--------------------------------------------

int M_Data[20][4] = {
  {0, 0, 320, 44},// メインメニューのタイトル座標
  {50, 48, 270, 44},// Menu 1
  {50, 96, 270, 44},// Menu 2
  {50, 144, 270, 44},// Menu3
  {50, 192, 270, 44}//  Menu4
};
//ボタン指定の変数

#define M_x    0
#define M_y    1
#define M_w    2
#define M_h    3

//メニュー操作三角ボタン座標
#define P1_x    0
#define P1_y    94
#define P2_x    20
#define P2_y    50
#define P3_x    40
#define P3_y    94

#define B1_x    0
#define B1_y    145
#define B2_x    40
#define B2_y    125
#define B3_x    40
#define B3_y    165

#define N1_x    0
#define N1_y    194
#define N2_x    40
#define N2_y    194
#define N3_x    20
#define N3_y    238


//------------------------------------
//  タッチメニューの項目名の定義
//------------------------------------
String MainMenu[20] = {
  "10",// 全メニュー数を文字変数で指定
  "Menu Choice Function",//最初にメインのメニュータイトルを記述
  "Menu A1",//以降、メニュー名を順次記述
  "Menu A2",
  "Menu A3",
  "Menu A4",
  "Menu A5",
  "Menu A6",
  "Menu A7",
  "Menu A8",
  "Menu A9",
  "Menu A10"
};


//------------------------------------------
//  Disp_4Menu用Global変数
//------------------------------------------

int Touch = 0;//  タッチメニュー選択（＝０メニュー、＝１キーボード）  <<<<<<<<<<<<<<<<<<<<<<<<<<<   1/4
int TSM = 0;//  総サブメニュー数（配列から読み込む）
int PT = 0;//  メニューの配列を読み出すポインター
int SMCP = 1;//  ページ内のサブメニュー表示済み個数＋１
int LPGM = 0;//  最終ページの表示メニュー数＋１
int TSMC = 0;//  既に表示したメニューの数（表示したら＋１）
int Btn_Flg = 0;//  =1　Prev, =2 Next, =3 Back ボタン押下で番号を入れる
int Menu_Flg = 0;// =0, =1 ,,,,  =4  Menuボタン押下
int Next_Flg = 0;// ボタン使用許可（=0, =1）
int Prev_Flg = 0;
int Back_Flg = 1;
int MN0_Flg = 0;// ボタン使用許可（=0, =1）
int MN1_Flg = 0;
int MN2_Flg = 0;
int MN3_Flg = 0;
int MN4_Flg = 0;
int R0 = 6;;//  角丸四角形の角の半径


//==========================================================
//  メニュー表示選択アプリ関連関数
//==========================================================
void Set_Menu_Col(void) {
  Mbc = Orange;
  Mfc = White;
  Sbc = ForestGr;
  Sfc = White;
  Rbc = MidNiBlue;
  Rfc = White;
}
//-------------------------------------------
//  指定されたメニューを指定行に表示する
//-------------------------------------------
void Menu_Set(int M_No, String Menu_Name, uint32_t Menu_Col, uint32_t Menu_Bkc) {

  int Off_x, Off_y;
  R0 = 6;

  lcd.setTextSize(2);// 文字サイズ指定（文字サイズ１はかなり小さい）
  lcd.drawRoundRect(M_Data[M_No][M_x], M_Data[M_No][M_y], M_Data[M_No][M_w], M_Data[M_No][M_h], R0, 0xffffffU);//タッチボタンのエリアを表示
  lcd.fillRoundRect(M_Data[M_No][M_x] + 1, M_Data[M_No][M_y] + 1, M_Data[M_No][M_w] - 2, M_Data[M_No][M_h] - 2, R0, Menu_Bkc);
  Off_x = (M_Data[M_No][M_w]  - (Menu_Name.length() * 12 )) / 2 + 2;
  Off_y = (M_Data[M_No][M_h] - 16) / 2 + 1;
  lcd.setCursor(M_Data[M_No][M_x] + Off_x, M_Data[M_No][M_y] + Off_y);
  lcd.setTextColor(Menu_Col, Menu_Bkc );//文字白色と背景色セット
  lcd.println(Menu_Name);
}

//--------------------------------
//  Previous Button のOn/Off設定
//--------------------------------
void Prev_on(void) {
  lcd.fillTriangle( P1_x, P1_y, P2_x, P2_y, P3_x, P3_y, Purple ); // ３点間の三角形の塗り
  lcd.drawTriangle( P1_x, P1_y, P2_x, P2_y, P3_x, P3_y, White ); // ３点間の三角形の塗り
  Prev_Flg = 1;
}
void Prev_off(void) {
  lcd.fillTriangle( P1_x, P1_y, P2_x, P2_y, P3_x, P3_y, Black ); // ３点間の三角形の塗り
  Prev_Flg = 0;
}

//--------------------------
//  Next Button のOn/Off設定
//--------------------------
void Next_on(void) {
  lcd.fillTriangle( N1_x, N1_y, N2_x, N2_y, N3_x, N3_y, Purple ); // ３点間の三角形の塗り
  lcd.drawTriangle( N1_x, N1_y, N2_x, N2_y, N3_x, N3_y, White ); // ３点間の三角形の塗り
  Next_Flg = 1;
}
void Next_off(void) {
  lcd.fillTriangle( N1_x, N1_y, N2_x, N2_y, N3_x, N3_y, Black ); // ３点間の三角形の塗り
  Next_Flg = 0;
}

//-------------------------
//  Menu Flg Clear
//-------------------------
void Menu_Flg_Clear(void) {
  Btn_Flg = 0;
  MN0_Flg = 0;// ボタン使用許可（=0, =1）
  MN1_Flg = 0;
  MN2_Flg = 0;
  MN3_Flg = 0;
  MN4_Flg = 0;
}

//-------------------------
//  Back Button のOn/Off設定
//-------------------------
void Back_on(void) {
  lcd.fillTriangle( B1_x, B1_y, B2_x, B2_y, B3_x, B3_y, Purple ); // ３点間の三角形の塗り
  lcd.drawTriangle( B1_x, B1_y, B2_x, B2_y, B3_x, B3_y, White ); // ３点間の三角形の塗り
  Back_Flg = 1;
}
void Back_off(void) {
  lcd.fillTriangle( B1_x, B1_y, B2_x, B2_y, B3_x, B3_y, Black ); // ３点間の三角形の塗り
  Back_Flg = 0;
}

//--------------------------------------------------------
//  タッチボタン別メニュー操作時の割り込み処理（FLGを立てる）
//--------------------------------------------------------

void T_Menu_Prev_B(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Btn_Flg = 1;
}
void T_Menu_Next_B(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Btn_Flg = 2;
}
void T_Menu_Back_B(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Btn_Flg = 3;
}
void T_Menu_MN0(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Menu_Flg = 0;
}
void T_Menu_MN1(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Menu_Flg = 1;
}
void T_Menu_MN2(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Menu_Flg = 2;
}
void T_Menu_MN3(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Menu_Flg = 3;
}
void T_Menu_MN4(TouchEvent& e) {
  if (e.type != TE_RELEASE) return;
  Menu_Flg = 4;
}


//---------------------------------------------------------
//  メニュー表示・タッチ入力関数（４行以上のメニューを表示する関数）
//  タッチボタン定義を組み入れる
//---------------------------------------------------------
int Disp_4Menu(String Menu_List[], uint32_t Tytle_C, uint32_t Tytle_Ch, uint32_t Menu_C, uint32_t Menu_Ch, uint32_t SMenu_C, uint32_t SMenu_Ch) {

  int Menu_No = 0;

  //----------------------------------------------
  //  タッチパネルボタンエリアの初期設定（ポインター変数）
  //----------------------------------------------
  //----------------------------------------------
  //  タッチパネルボタンエリアの初期設定
  //  定義した順番で検出されるので、重なったエリアの場合は
  //  先に定義したエリアのイベントが優先される
  //----------------------------------------------
  TouchButton *MN0 = new TouchButton(M_Data[0][M_x], M_Data[0][M_y], M_Data[0][M_w], M_Data[0][M_h], "MN0");// メニューボタン0
  TouchButton *MN1 = new TouchButton(M_Data[1][M_x], M_Data[1][M_y], M_Data[1][M_w], M_Data[1][M_h], "MN1");// メニューボタン1
  TouchButton *MN2 = new TouchButton(M_Data[2][M_x], M_Data[2][M_y], M_Data[2][M_w], M_Data[2][M_h], "MN2");// メニューボタン2
  TouchButton *MN3 = new TouchButton(M_Data[3][M_x], M_Data[3][M_y], M_Data[3][M_w], M_Data[3][M_h], "MN3");// メニューボタン3
  TouchButton *MN4 = new TouchButton(M_Data[4][M_x], M_Data[4][M_y], M_Data[4][M_w], M_Data[4][M_h], "MN4");// メニューボタン4
  //-----------------------------------------------
  //  メニューの次ページ前ページ操作ボタン設定
  //-----------------------------------------------
  TouchButton *Prev_B = new TouchButton(P1_x, P2_y, P3_x, P3_y - P2_y, "Prev_btn");// メニュー次ボタン
  TouchButton *Next_B = new TouchButton(N1_x, N1_y, N2_x, N3_y - N2_y, "Next_btn");// メニュー戻タン
  TouchButton *Back_B = new TouchButton(B1_x, B2_y, B2_x, B3_y - B2_y, "Back_btn");// メニュー戻タン
  //--------------------------------------
  //  タッチメニュー割り込み処理関数登録
  //--------------------------------------
  Prev_B->addHandler(T_Menu_Prev_B, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  Next_B->addHandler(T_Menu_Next_B, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  Back_B->addHandler(T_Menu_Back_B, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  MN0->addHandler(T_Menu_MN0, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  MN1->addHandler(T_Menu_MN1, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  MN2->addHandler(T_Menu_MN2, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  MN3->addHandler(T_Menu_MN3, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  MN4->addHandler(T_Menu_MN4, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  //------------------------------
  //  画面クリア
  //------------------------------

  //lcd.init();// ここに記述しないと起動しない！！
  lcd.setTextDatum(0);
  lcd.setTextFont(1);
  lcd.setTextSize(2);

  lcd.fillScreen(0);  // 黒で塗り潰し
  TSM = Menu_List[PT].toInt();//  配列の最初にあるサブメニューの総個数をTSMにセットする
  PT++;
  LPGM = (TSM % 4) + 1;//  最終メニューページのサブメニュー数（＋１）を調べる
  Menu_Set(0, Menu_List[PT], Tytle_Ch, Tytle_C ); //  メインタイトル表示
  PT++;
  Menu_Flg_Clear();
  Back_on();
  R0 = 6;

One_Menu_Disp:

  if (!(TSMC < TSM ))goto Page_Stop;
  if (!(SMCP <= 4 )) goto Page_Stop;
  Menu_Set(SMCP, Menu_List[PT], Menu_Ch, Menu_C);

  //----  メニューのタッチエリア検出の許可設定  ----
  switch (SMCP) {
    case 1:
      MN1_Flg = 1;
      break;
    case 2:
      MN2_Flg = 1;
      break;
    case 3:
      MN3_Flg = 1;
      break;
    case 4:
      MN4_Flg = 1;
      break;
      break;
  }

  //-------------------
  //  各カウンターを＋１
  //-------------------
  PT++;//  配列ポインター＋１
  SMCP++;//  ページ内サブメニューカウンター＋１
  TSMC++;//  サブメニュートタルカウンター＋１
  goto One_Menu_Disp;

  //------------------------------
  //  ４行メニュー表示完了
  //------------------------------
Page_Stop:
  //------------------
  //  ボタン表示判断処理
  //------------------

  if (TSM <= 4) {//  １ページ以内のメニュー
    Next_off();
    Prev_off();
    goto Scan;
  }
  if (TSMC >= TSM) {//  最終ページ
    Next_off();
    Prev_on();
    goto Scan;
  }
  if (TSMC >= 5 ) {//  中間ページ
    Next_on();
    Prev_on();
    goto Scan;
  }
  if (TSMC <= 5) {//  最初のページ（メニュー数５以上）
    Next_on();
    Prev_off();
    goto Scan;
  }

  //--------------------------
  //  ボタンタッチ検出と処理
  //--------------------------

Scan:

  M5.update();//タッチパネル状態変化検出

  //------  Nextボタン  ----------
  if (( Btn_Flg == 2) && ( Next_Flg == 1 ))
  {
    delay(100);
    SM_Clear();
    SMCP = 1;//  ページ内のサブメニュー表示済み個数＋１
    Menu_Flg_Clear();
    goto One_Menu_Disp;
  }

  //------  Prevボタン  ----------
  if (( Btn_Flg == 1) && ( Prev_Flg == 1 ))
  {
    delay(100);
    SM_Clear();
    TSMC = TSMC - SMCP - 3;
    PT = PT - SMCP - 3;
    SMCP = 1;//  ページ内のサブメニュー表示済み個数＋１
    Menu_Flg_Clear();
    goto One_Menu_Disp;
  }

  //------  Back ボタン  ----------
  if (( Btn_Flg == 3) && ( Back_Flg == 1 ))
  {
    delay(100);
    lcd.fillScreen(0);  // 黒で塗り潰し
    Menu_No = 0;

    goto Button_Clear;

  }

  //------  Menuボタン  -----------------------------------------------------------
  //  メニュー表示したらメニューボタンFLGをセット１－４し
  //  メニュータッチされたら、タッチボタンFLGをセット
  //------------------------------------------------------------------------------

  //--------------  Menu Button 1 ----------------
  if (( Menu_Flg == 1) && ( MN1_Flg == 1 ))
  {
    //  タッチしたメニューをハイライトさせる
    Menu_No = (TSMC - 1 ) / 4 * 4 + Menu_Flg;
    Menu_Set(Menu_Flg, Menu_List[Menu_No + 1], SMenu_Ch, SMenu_C);
    delay(1000);
    goto Button_Clear;


  }
  //--------------  Menu Button 2 ----------------
  if (( Menu_Flg == 2) && ( MN2_Flg == 1 ))
  {
    //  タッチしたメニューをハイライトさせる
    Menu_No = (TSMC - 1 ) / 4 * 4 + Menu_Flg;
    Menu_Set(Menu_Flg, Menu_List[Menu_No + 1], SMenu_Ch, SMenu_C);
    delay(1000);
    goto Button_Clear;

  }
  //--------------  Menu Button 3 ----------------
  if (( Menu_Flg == 3) && ( MN3_Flg == 1 ))
  {
    //  タッチしたメニューをハイライトさせる
    Menu_No = (TSMC - 1 ) / 4 * 4 + Menu_Flg;
    Menu_Set(Menu_Flg, Menu_List[Menu_No + 1], SMenu_Ch, SMenu_C);
    delay(1000);
    goto Button_Clear;
  }
  //--------------  Menu Button 4 ----------------
  if (( Menu_Flg == 4) && ( MN4_Flg == 1 ))
  {
    //  タッチしたメニューをハイライトさせる
    Menu_No = (TSMC - 1 ) / 4 * 4 + Menu_Flg;
    Menu_Set(Menu_Flg, Menu_List[Menu_No + 1], SMenu_Ch, SMenu_C);
    delay(1000);
    goto Button_Clear;
  }
  goto Scan;

Button_Clear:
  lcd.fillScreen(0);  // 黒で塗り潰し
  delete(MN0);// メニュー表示アプリの定義されたボタンを削除。
  delete(MN1);
  delete(MN2);
  delete(MN3);
  delete(MN4);
  delete(Prev_B);
  delete(Next_B);
  delete(Back_B);

  return Menu_No;//選択したメニュー番号を返して、このメニュー処理関数を終了する
}

//-------------------------------------------------
//  サブメニュー表示エリア消去
//-------------------------------------------------
void SM_Clear(void) {
  lcd.fillRect(50, 48 , 270, 192, 0x000000U);
}



//====================================================================================================
//  SetUpブロック
//====================================================================================================

void setup() {

  //-------------------------
  //  M5Stackの初期化
  //-------------------------

  //------------------------------
  //  初期化
  //------------------------------
  //M5.begin(true, false, true, true);//タッチキーボード
  M5.begin();
  Serial.begin(115200);
  Serial.println("Start");
  Serial.println("M5_Core_ESP32");
  SerialBT.begin("M5Core2", true);//自分の名前でMasterとして接続
  SerialBT.flush();

  //-------------------------
  //  LovyanGFX初期設定
  //-------------------------
  lcd.init();
  // 回転方向を 0～3 の4方向から設定します。(4～7を使用すると上下反転になります。)
  lcd.setRotation(1);
  // バックライトの輝度を 0～255 の範囲で設定します。
  lcd.setBrightness(255); // の範囲で設定
  lcd.setTextFont(2);//Font
  lcd.setTextSize(1);// 文字サイズ指定（文字サイズ１はかなり小さい）
  // カラーモード設定
  lcd.setColorDepth(24);  // RGB888の24ビットに設定(表示される色数はパネル性能によりRGB666の18ビットになります)
  // clearまたはfillScreenで画面全体を塗り潰します。
  // どちらも同じ動作をしますが、clearは引数を省略でき、その場合は黒で塗り潰します。
  lcd.fillScreen(0);  // 黒で塗り潰し
  // カーソルセット
  lcd.setCursor(0, 0);
  lcd.setTextColor(0xffffffU, 0x000000U);//文字白色と背景黒色セット
  // LovyanGFX 設定完了メッセージ
  lcd.println("M5Stack Core2");
  lcd.println("LovyanGFX");
  lcd.println("New Touch Menu Appl Demo");
  lcd.println("M5 Menu_App09");
  delay(2000);
  Set_Menu_Col();//Menuの色指定セットアップ




  //------------------------------------
  //  Bluetoothのセットアップとペアリング
  //------------------------------------
  String MACadd = "24:0A:C4:EA:39:D2";//接続先のESP32のBTアドレス
  uint8_t address[6]  = {0x24, 0x0A, 0xC4, 0xEA, 0x39, 0xD2};// Slave ESP32
  String name = "ESP32";
  char *pin = "1234"; //<- standard pin would be provided by default
  bool connected;

  Serial.println("The M5device started in master mode, make sure remote BT device is on!");

  // connect(address) is fast (upto 10 secs max), connect(name) is slow (upto 30 secs max) as it needs
  // to resolve name to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices bluetooth address and device names
  //connected = SerialBT.connect(name);

  connected = SerialBT.connect(address);//ESP32のアドレスで接続トライ

  delay(2000);
  lcd.fillScreen(0);
  lcd.setCursor(0, 0);
  lcd.println("Connecting");
  delay(2000);
  if (connected) {
    Serial.println("Connected Succesfully!");
    lcd.println("Connected Succesfully!");
  } else {
    Serial.println("Connection ERR, reconnect...");
    lcd.println("Connection ERR, reconnect...");
    while (!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
      lcd.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
    }
  }

  lcd.println("Ready to go");// ペアリング完了。通信可能。
  delay(1000);

}

//==============================================================================================
//  Loopブロック
//==============================================================================================

void loop() {
  int Result = 0;

  Btn_Flg = 0;//  =1　Prev, =2 Next, =3 Back ボタン押下で番号を入れる
  Menu_Flg = 0;// =0, =1 ,,,,  =4  Menuボタン押下
  TSM = 0;//  総サブメニュー数（配列から読み込む）
  PT = 0;//  メニューの配列を読み出すポインター
  SMCP = 1;//  ページ内のサブメニュー表示済み個数＋１
  LPGM = 0;//  最終ページの表示メニュー数＋１
  TSMC = 0;//  既に表示したメニューの数（表示したら＋１）

  //------------------------------------------
  //  繰り返しの処理用にLCDの初期化
  //------------------------------------------
  lcd.fillScreen(0);  // 黒で塗り潰し
  lcd.setTextColor(0xffffffU, 0x000000U);//文字白色と背景黒色セット
  lcd.setTextSize(2);

  //------------------------------------------
  //  メニューアプリ：選択したメニュー番号を返す
  //------------------------------------------
  Result = Disp_4Menu(MainMenu, Mbc, Mfc, Sbc, Sfc, Rbc, Rfc );//<<<<<<<<<<<<<  メニュー動作時に指定  No3
  lcd.fillScreen(0);  // 黒で塗り潰し
  lcd.setCursor(0, 0);
  lcd.setTextColor(0xffffffU, 0x000000U);//文字白色と背景黒色セット
  lcd.setTextDatum(0);
  lcd.setTextFont(1);
  lcd.setTextSize(2);
  lcd.print("You'v touched Menu No# ");//タイトル表示;
  lcd.println(Result);// 選択したメニュー番号を返す

  SerialBT.println(Result);//選択されたメニュー番号を文字列として送信。

  delay(1000);//以下ループ動作する



}


//===================  End of Application ====================================
