// GPS Logger version 4.0 lite

// lite version (without M096P4BL) of GPS logger 4

/* with
   u-Blox GM-8013T
   SDcard
*/

/* note
  Arudino-SDcard
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module. ->10
*/

/* Arduino - GM-8013T (with software serial)
    RX: 8 - 3 (TTL output)
    TX: 9 - 4 (TTL input)
*/

/* flow
  初期化ステップ1(シリアルポート、ソフトウェアシリアル、OLEDの初期化)
  初期化ステップ2(GPSの設定：データの受信間隔・GPSより出力/ログするセンテンスの設定)
  初期化ステップ3(SD・ファイルの初期化)
  データを受信し、GPRMCもしくはGNRMCセンテンスのチェック→有効なRMCセンテンスならば日付を得る
  日付、および既存ファイルと重複しないようファイル名を決定
  ファイル作成日を指定していったんファイルをopen→close

  loop
  データ受信
  LED点灯
  ファイルが有効ならば
    ファイルをopen
    データをファイルに記録(シリアルに出力)
    ファイルをclose
    LED消灯
  GPRMCもしくはGNRMCセンテンスのチェック、RMCセンテンスならば
    有効なRMCセンテンスならば
      validのindicator をOLEDに表示
      緯度・経度・日付・時刻を得て、OLEDに情報を表示
    でなければ
      invalid dataであることの表示
  loop end

*/

// uncomment if debug and use serial
#define DEBUG_SERIAL

#include <SD.h>
#include <SoftwareSerial.h>
#include <MsTimer2.h>

#define SD_CHIP_SELECT 10
#define TimeZone (9)
#define SW_PIN_NO 6
#define LED_PIN_NO 7
#define BUFSIZE 200

// initialize the library with the numbers of the interface pins
SoftwareSerial gps(8, 9); // RX, TX

File logFile;

char strbuf[BUFSIZE];
char filename[13];
bool fileEnable;
bool runMode;
bool logFileOpened;

void setup()
{
#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  delay(1000);
  Serial.println("GPS Logger Start");
#endif

  pinMode(LED_PIN_NO, OUTPUT) ;      // LEDに接続
  pinMode(SW_PIN_NO, INPUT_PULLUP ) ; // SW に接続し内部プルアップに設定

  // initialize and GPS
  delay(1000);
  gps.begin(9600); // Software Serial
  // configure output of GM - 8013T
  configure_GP8013T();
  
#ifdef DEBUG_SERIAL
  Serial.println("GPS ready");
#endif    

  delay(1000);
  
// initialize SD card
  filename[0]='\0';
  pinMode(SD_CHIP_SELECT, OUTPUT);
  if (SD.begin(SD_CHIP_SELECT)) {
    fileEnable = checkSDFile();
  } else {
    fileEnable=false;
  }
  logFileOpened=false;
  delay(2000);

  // enable timer2
  MsTimer2::set(60000, flushSD);
  MsTimer2::start();

  runMode = 0;
}

void loop()
{
  int swStatus;

  swStatus = digitalRead(SW_PIN_NO);
  if (runMode == 1 && swStatus == LOW) {
    runMode = 0; // logging turn off
#ifdef DEBUG_SERIAL
  Serial.println("Logging stopped");
#endif    
    digitalWrite(LED_PIN_NO, LOW);
    if (logFileOpened == true) {
      logFile.close();
      logFileOpened = false;
    }
  } else if (runMode == 0 && swStatus == HIGH) { // logging turn on
    runMode = 1;
    if (logFileOpened == false) {
      if(!fileEnable) {
        fileEnable = checkSDFile();
      }
      if(fileEnable) {
        logFile = SD.open(filename, FILE_WRITE);
      }
      
      if(logFile & fileEnable) {
        logFileOpened=true;
        digitalWrite(LED_PIN_NO, HIGH);
#ifdef DEBUG_SERIAL
        Serial.println("Logging Started.");
#endif
      }
    }
  }
  doLogging();
}

void doLogging()
{

  if (gps.available()) {  // if recived serial signal
//    digitalWrite(LED_PIN_NO,HIGH);
    recvStr();   // read serial data to string buffer
#ifdef DEBUG_SERIAL
  Serial.print(strbuf);
#endif
    if (logFileOpened == true) {
      logFile.print(strbuf);
//      logFile.flush();
    }
  }
}

// recieve string from GPS
void recvStr()
{
  int i = 0;
  char c;
  while (1) {
    if (gps.available()) {
      c = gps.read();
      strbuf[i] = c;
      if (c == '\n') break;
      i++;
    }
  }
  i++;
  strbuf[i] = '\0';  // \0: end of string
}

bool checkSDFile()
{
    for (unsigned int index = 0; index < 65535; index++) {
        char fileTmp[13];
        sprintf(fileTmp, "GPS%05d.TXT", index);
        if (!SD.exists(fileTmp)) {
            logFile = SD.open(fileTmp, FILE_WRITE);
            Serial.println(fileTmp);
            if (logFile) {
                Serial.println("Log file opened");
                strcpy(filename,fileTmp);
                logFile.close();
                return true;
            }
            Serial.println("Can't open logfile");
            break;
        }
    }
    return false;
}

void send_PUBX_packet(const char *p)
{
  uint8_t checksum = 0;
  gps.print('$');
  do {
    char c = *p++;
    if (c) {
      checksum ^= (uint8_t)c;
      gps.print(c);
    }
    else {
      break;
    }
  }
  while (1);
  gps.print('*');
  gps.println(checksum, HEX);
}

// $GNRMC, $GNVTG, $GNGGA, $GPGSV, $GLGSV, $GNGLL, $GNGSA
void configure_GP8013T()
{
  // set NMEA sentence output rate
  // "$PUBX,40", followed by
  // msgID, 
  // output rate on DDC
  // output rate on USART1
  // output rate on USART2
  // output rate on USB
  // output rate on SPI
  // 0 (reserved)
  // and "*" + checksum + CRLF
  send_PUBX_packet("PUBX,40,RMC,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,VTG,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GGA,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GSV,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GLL,0,5,0,0,0,0");
  send_PUBX_packet("PUBX,40,GSA,0,5,0,0,0,0");
}

void flushSD()
{
  if (logFileOpened == true) {
    digitalWrite(LED_PIN_NO, LOW);
    logFile.flush();
    delay(500);
    digitalWrite(LED_PIN_NO, HIGH);
  }
}
