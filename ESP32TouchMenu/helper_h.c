/*
    This helper file contains all the function calls required by the "OS"

*/
#ifndef _HELPER_H
#define _HELPER_H

//  DEFINES **************************************************
#define CYCLE_DURATION 5000
#define HDC1080_ADDR 0x40
#define MAX30102_ADDR 0x57
#define CCS811_ADDR 0x5A    //Alternate I2C Address
#define MLX90614_ADDR 0x5B
#define SI1145_ADDR 0x60
#define MPU6050_ADDR 0x68
#define BME280_ADDR 0x76
#define MLX90640_ADDR 0x33
#define TCS34725_ADDR 0x29

//  For the ESP32 WROOM 32, these are the default GPIOs.
//  Display - updated Feb.,26,2021 - to adjust for SD Card on SPI bus GPIO5 is now SD_CS as per library.
#define TFT_DC          27  //GPIO27
#define TFT_CS          15   //GPIO15
#define TFT_MOSI        23  //GPIO23  - VSPI-MOSI
#define TFT_CLK         18  //GPIO18  - VSPI CLK
#define TFT_RST         4   //GPIO4 
#define TFT_MISO        19  //GPIO19  - VSPI-MISO
#define TFT_BLANK       8   //GPIO8
//  new for blanking tft via MOSFET - GPIO6
//**********************************************************
//Bluetooth Low Energy

#define SERVICE_UUID        "4a41d6bc-24ed-4a80-b3b4-dc1334f5c59f"
#define CHARACTERISTIC_UUID "0b48fbda-ec20-45f7-9f6b-908c7a4d17da"

//**********************************************************
#define BUILTIN_LED   2

//  Touch SPI
#define _sclk     25  //GPIO25
#define _mosi     32  //GPIO32
#define _miso     39  //GPIO39
#define TOUCH_CS  33  //GPIO33

#define SEALEVELPRESSURE_HPA (1013.25)

#define TFT_LANDSCAPE             1
#define TFT_PORTRAIT_NORMAL       2
#define TFT_LANDSCAPE_INVERT      3
#define TFT_PORTRAIT_INVERT       4

#define border_colour ILI9341_YELLOW
#define num_button_colour ILI9341_RED
#define key_button_colour ILI9341_WHITE
#define yes_no_button_colour ILI9341_GREEN
#define num_text_colour ILI9341_BLACK
#define MENU 0
#define ALPHA 1
#define NUMERIC 2
#define YESNO 3

int GLOBAL_NUMBER = 0;      //  this is the holder for numeric input.
String GLOBAL_STRING = "";  //  this is for the global string.
int GLOBAL_PAD_XPOS = 0;    //  holder for the X-POS position of the pad.
int GLOBAL_PAD_YPOS = 0;    //  Y-POS, used to calc the touch offset position.
int GLOBAL_TOUCH_TYPE = 0;  //  this tells the system what to look for menu,alpha,numeric,boolean(y/n).


//  VARS  ********************************************************
bool DEBUG = false;
char *ssid     = "****";      //  add your own credentials.
char *password = "********";  //  here.
const char* ntpServer = "pool.ntp.org";
const int LOCAL_TIME_OFFSET = -4;
const long utcOffsetInSeconds = 3600 * LOCAL_TIME_OFFSET;
bool Century = false, h12, pm;
char daysOfTheWeek[7][20] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char months[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
int dow;
int counter = 1;
int groupMode = 0;
//BLE
bool deviceConnected = false;
bool oldDeviceConnected = false;

// our RGB -> eye-recognized gamma color - TCS 34725
byte gammatable[256];
// Your GPRS credentials (leave empty, if not needed)
const char apn[]      = ""; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = "";

double ATC, OTC, ATF, OTF;
const uint8_t TFT_ORIENTATION = TFT_LANDSCAPE_INVERT;
int GFIPZ = 3;           //  values from 1 to 4. 1 - 1:1, 2 - 2:1, 3 - 3:1, 4 - 4:1.
const byte RATE_SIZE = 4; //  Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];    //  Array of heart rates.
byte rateSpot = 0;
long lastBeat = 0;        //  Time at which the last beat occurred.
float beatsPerMinute;
int beatAvg;
// Touch/Menu stuff ******************
//add to your menus here
/*
        1   main 1
        2   main 2
        3   main 3
        4   flir 1
        5   flir 2  - other
        6   flir 3  - zoom
        7   flir 4  - colour select
        7   time 1
        8   network 1
        9   news 1
        10  medical 1
        11  services 1
*/
const int M_M1 = 0;
const int M_M2 = 1;
const int M_M3 = 2;
const int M_FLIR = 3;
const int M_FLIR2 = 4;
const int M_FLIR_ZOOM = 5;
const int M_FLIR_COLOUR = 6;
const int M_TIME = 7;
const int M_NETWORK = 8;
const int M_NEWS = 9;
const int M_MEDICAL = 10;
const int M_SERVICES = 11;

const int NOB = 5;//number of buttons.
const int NOM = 12;// number of menus
String main[NOM][NOB] = {{"WEATHER", "TIME", "FLIR", "NEWS", "MORE..."}, {"NETWORK", "E-MAIL", "MEDICAL", "ADMIN", "MORE..."},

  {"BLUETOOTH", "GYRO", "RGB", "SERVICES", "BACK..."}, {"MODE", "RESOLUTION", "STATUS", "ON/OFF", "MORE..."},

  {"ZOOM", "COLOUR", "***", "***", "BACK..."}, {"1:1", "2:1", "3:1", "4:1", "BACK..."},

  {"COLOUR 1", "COLOUR 2", "COLOUR 3", "COLOUR 4", "COLOUR 5"}, {"SET TIME", "SET ALARM", "CLEAR", "INFO", "BACK..."},

  {"RECONNECT", "DISCONNECT", "SELECT", "SCAN", "BACK..."}, {"SPORTS", "LIFESTYLES", "TECH", "WORLD", "BACK..."},

  {"OXIMETER", "BODY TEMP", "SPIROMETER", "***", "BACK..."}, {"HARDWARE", "KILL I/O", "RESTORE", "***", "BACK..."}
};
int MENU_MODE;
bool PAD_DOWN = false;//the interrupt returns these values.
int TouchX = 0;
int TouchY = 0;
const int TOUCH_IRQ = 35;// to identify the pin to watch.

int BUTTON_NUMBER = 0;      //  this is for the selected menu buttons. 0- 4.
#define IDLE 0
#define ENVIRONMENT 1       //  all of the components attached to the ESP32.
#define OXIMETER 2          //  used to run the automation.
#define FLIR 3
#define RGB_SCAN 4
#define GYRO 5
#define TIME 6
#define NETWORK 7
int SystemMode = IDLE;         //  manages which unit is running. 0 means idle.
//  Status and Data from sensors initial values
//  CJMCU 8128 module
bool BME280_READY = false;
bool BME280_ACTIVE = false;
bool CCS811_READY = false;
bool HDC1080_READY = false;
//  GY1145 Light  Quality
bool LIGHTQ_READY = false;
bool LIGHTQ_ACTIVE = false;
//  MLX90614 ir
bool IR_READY = false;
bool IR_ACTIVE = false;
//  MAX30102 oxi
bool OXI_READY = false;
bool OXI_ACTIVE = false;
//  RTC
bool RTC_READY = false;
bool RTC_ACTIVE = false;
// Flir
bool FLIR_READY = false;
bool FLIR_ACTIVE = false;
//  GY-521  - gyro/accel
bool MPU6050_READY = false;
bool MPU6050_ACTIVE = false;
// TCS34725 rgb sensor
bool TCS34725_READY = false;
bool TCS34725_ACTIVE = false;
//  INITIALIZATION ******************************************************
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
Adafruit_SI1145 light_data = Adafruit_SI1145();
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_MLX90640 flirCam;
Adafruit_MPU6050 myGyro;

float flirFrame[32 * 24];     // buffer for full frame of temperatures

XPT2046_Touchscreen touch(TOUCH_CS);
TS_Point rawLocation;
CCS811 myCCS811(CCS811_ADDR);
ClosedCube_HDC1080 myHDC1080;
BME280 myBME280;
MAX30105 particleSensor;

Adafruit_TCS34725 rgbSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


//Logo2 and Logo3 are two bmp pictures that display on the TFT if called
// these can be moved to a file(later)
static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,
  0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
  0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
  0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,
};
static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
  0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
  0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
  0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00
};

/*
   remember that when adding code to this header is to keep your functions and variables
   "visible" to one another.
   ie don't go  C=A+B
                A=1
                B=2
   do it like so...
                A=1
                B=2
                C=A+B
   this is because the compiler only looks in one direction (no forward referencing)
   typical complier error "scope" issues are related to this.

  FYI list of functions *************************************************************************
  IRAM_ATTR ScreenPressed();
  AttachTouch();
  InitDisplay();
  infoBlocks();
  clearAll();
  clearBody();
  clearStatus();
  showStatus();
  ClearScreen();
  showBiosInfo();
  syncronizeRTC();
  showDateTime();
  showMail();
  initMyTouch();
  scanI2CBus();
  initializeSensorGroup();
  showMenuSystem(int m);
  showAlphaPad(int x, int y);
  showNumPad(int x, int y);
  showYesNo(int x, int y);
  bool checkYesNo(int x, int y);
  ShowIRResults();
  oxiTest();
  showWeather();
  showLightData();
  showWifi();
  connectToNetwork();
  disconnectFromNetwork();
  showPhone();
  showNews();
  showAdmin();
  showStars();
  int menuButtonCheck(int a, int b);
  parseTouchData(int x, int y);
  action(int bp);
*/
#include "Pads.h"
//  FUNCTIONS & ROUTINES  ***********************************************
// The almighty RAM LOAD INTERRUPT(someone touched me and I know where!) TouchX,TouchY,PAD_DOWN gets updated
void IRAM_ATTR ScreenPressed() {
  // halt further interrupts
  detachInterrupt(TOUCH_IRQ);
  if ( touch.touched() )
  {
    rawLocation = touch.getPoint();
    TouchX = rawLocation.x;
    TouchY = rawLocation.y;
    PAD_DOWN = true;
  }
  digitalWrite(BUILTIN_LED, HIGH);
  // OK good to go...
  attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ), ScreenPressed, FALLING);
}
//This is to re-enabe the touch interrupt
void AttachTouch() {
  PAD_DOWN = false;
  attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ), ScreenPressed, FALLING);
}

void InitDisplay() {
  tft.begin(60000000);
  delay(10);
  tft.setRotation(TFT_ORIENTATION);
  tft.fillScreen(ILI9341_BLACK);
}


void infoBlocks() {
  tft.fillRoundRect(5, 5, 230, 25, 10, ILI9341_WHITE);
  tft.fillRoundRect(7, 7, 226, 21, 8, ILI9341_BLACK);
  tft.fillRoundRect(5, 35, 230, 200, 10, ILI9341_WHITE);
  tft.fillRoundRect(7, 37, 226, 196, 8, ILI9341_BLACK);
}
void clearAll() {
  tft.fillRoundRect(7, 7, 226, 21, 8, ILI9341_BLACK);
  tft.fillRoundRect(7, 37, 226, 196, 8, ILI9341_BLACK);
}
void clearBody() {
  tft.fillRoundRect(7, 37, 226, 196, 8, ILI9341_BLACK);
}
void clearStatus() {
  tft.fillRoundRect(7, 7, 226, 21, 8, ILI9341_BLACK);
}
/*
   show status will have two colours to display red and yellow
*/
void showStatus(String z) {
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.fillRoundRect(7, 7, 226, 21, 8, ILI9341_BLACK);
  tft.setCursor(15, 15);
  tft.print(z);
}
//  Clear Screen! just so i don't have to type blah blah blah! (macro)
void ClearScreen() {
  tft.fillScreen(ILI9341_BLACK);
}
void initSDCard() {
  if (!SD.begin()) {
    showStatus("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    showStatus("No SD card attached");
    return;
  }

  showStatus("SD Card Type: ");
  if (cardType == CARD_MMC) {
    showStatus("MMC");
  } else if (cardType == CARD_SD) {
    showStatus("SDSC");
  } else if (cardType == CARD_SDHC) {
    showStatus("SDHC");
  } else {
    showStatus("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  showStatus("SD Card Size: %lluMB\n" + cardSize);
}
void InitBLE() {

}

void showBiosInfo() {
  ClearScreen();
  tft.setCursor(0, 0);
  tft.drawRoundRect(0, 0, 320, 240, 10, border_colour);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setCursor(65, 10);
  tft.print("ETMS - CAPRICORN SOFTWARE - 2021");
  tft.setCursor(90, 20);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("BIOS V1.02.6 - 2/15/2021");
  tft.setCursor(135, 30);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.println("-System-");
  tft.setCursor(100, 40);
  tft.setTextColor(ILI9341_MAGENTA, ILI9341_BLACK);
  tft.print("ESP WROOM 32 240MHz");
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(30, 50);
  tft.print("CPU Dual Core..Pass");
  tft.setCursor(30, 60);
  tft.print("Memory.........Pass");
  tft.setCursor(180, 50);
  tft.print("ILI9341........Pass");
  tft.setCursor(180, 60);
  tft.print("XPT2046........Pass");

}
void deactivateAll() {
  bool BME280_READY = false;
  bool MPU6050_READY = false;
  bool TCS34725_READY = false;
  bool LIGHTQ_READY = false;
  bool OXI_READY = false;
  bool IR_READY = false;
  bool FLIR_READY = false;
  bool RTC_READY = false;
}
//not working - FIXME:
void syncronizeRTC() {
  if ( WiFi.status() == WL_CONNECTED) {
    showStatus("Fetching time from server..");
    configTime(3600, 3600, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      showStatus("Failed to obtain time");
      return;
    }
    tft.setCursor(50, 50);
    tft.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  } else {
    showStatus("No network connection");
  }
}
void showDateTime() {

}

/*
   this is for the touch controller XPT2046
   divide the clock by 1 - bus speed 60Mhz ??
*/
void initMyTouch() {
  SPI.begin(_sclk, _miso, _mosi);
  SPI.setClockDivider(1);
  SPI.beginTransaction(SPISettings(60000000, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
  touch.begin();
  showStatus("Touch Screen is ready!");
}
/*
  ScanI2CBus is implemented - 9/21/2020
  this has to be done before init sensor group
  sets the sensor availability so the system can ignore offline sensors
  to manage performance and no bad data woes.
*/
void scanI2CBus() {
  int lastRow = 220;
  byte error, address;
  int nDevices;
  Wire.begin();
  tft.setCursor(15, lastRow);
  tft.print("I2C Bus Scan:");
  for (address = 1; address < 127; address++ )
  {
    /*
      The i2c_scanner uses the return value of
      the Write.endTransmisstion to see if
      a device did acknowledge to the address.
    */

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      tft.setCursor(15, lastRow - 20);
      tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
      tft.print("I2C device at address 0x");
      if (address < 0x10) {
        tft.print("0");
      }
      tft.print(address, HEX);
      tft.print(" hex !");
      //tft.setCursor(60, lastRow);
      //tft.print(address);
      tft.setCursor(100, lastRow);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      if (address == 0x29) {
        tft.print("TCS34725 RGB Sensor  ");
        TCS34725_READY = true;
      }
      if (address == 0x33) {
        tft.print("MLX90640 Flir        ");
        FLIR_READY = true;
      }
      if (address == 0x40) {
        tft.print("HDC1080 Humidity     ");
        HDC1080_READY = true;
      }
      if (address == 0x57) {
        tft.print("MAX30102 Oxiometer   ");
        OXI_READY = true;
      }
      if (address == 0x5A) {
        tft.print("CCS811 Air Quality   ");
        CCS811_READY = true;
      }
      if (address == 0x60) {
        tft.print("Si1145 Light Quality ");
        LIGHTQ_READY = true;
      }
      if (address == 0x68) {
        tft.print("MPU6050 Gyro/Accel   ");
        MPU6050_READY = true;
      }
      if (address == 0x76) {
        tft.print("BME280 Pressure/Temp ");
        BME280_READY = true;
      }
      nDevices++;
      delay(500);
    }
    else if (error == 4)
    {
      tft.setCursor(20, lastRow);
      tft.print("Error at address 0x");
      if (address < 16)
        tft.print("0");
      tft.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    tft.setCursor(180, lastRow);
    tft.print("No I2C devices found");
  }
  else
  {
    tft.setCursor(15, lastRow);
    tft.print("I2C Bus Scan:");
    tft.setCursor(100, lastRow);
    tft.print("Done....            ");
  }
  delay(500);
}
/*
   This initialize handles most of the devices that is available
   If it finds a sensor it attempts to initialize or if the
   sensor is not attached it will flag the unit so the system
   will ignore it.
*/
void initializeSensorGroup() {
  int lineCount = 80;
  int itemStart = 40;
  int resutPos = 170;
  IR_READY = false;
  tft.setCursor(95, lineCount - 10);
  tft.print("-Sensors-");
  //start BME280
  myBME280.settings.commInterface = I2C_MODE;
  myBME280.settings.I2CAddress = 0x76;
  myBME280.settings.runMode = 3; //Normal mode
  myBME280.settings.tStandby = 0;
  myBME280.settings.filter = 4;
  myBME280.settings.tempOverSample = 5;
  myBME280.settings.pressOverSample = 5;
  myBME280.settings.humidOverSample = 5;
  myBME280.begin();

  delay(50);
  if (!myBME280.begin()) {
    BME280_READY = false;
  }
  //  RGB Sensor - TCS34725
  if (rgbSensor.begin()) {
    TCS34725_READY = true;
  } else {
    TCS34725_READY = false;
  }
  //Light sensor start
  if (!light_data.begin()) {
    LIGHTQ_READY = false;
  } else {
    LIGHTQ_READY = true;
  }

  //gyro sensor start
  if (MPU6050_READY) {
    // init gyro myGyro
    myGyro.begin(0x68);
    myGyro.setAccelerometerRange(MPU6050_RANGE_8_G);
    myGyro.setGyroRange(MPU6050_RANGE_500_DEG);
    myGyro.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }
  //light start
  light_data.begin(SI1145_ADDR);
  //humidity start
  myHDC1080.begin(0x40);
  delay(100);
  myCCS811.begin();


  delay(150);
  if (!myCCS811.begin()) {
    CCS811_READY = false;
  }
  //BME280 sensor results.
  lineCount += 10;
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("TEMP/PRESSURE");
  if (BME280_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }

  //flir test results.
  lineCount += 10;
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("FLIR");
  if (FLIR_READY) {
    flirCam.begin(MLX90640_I2CADDR_DEFAULT, &Wire);
    flirCam.setMode(MLX90640_CHESS);
    flirCam.setResolution(MLX90640_ADC_18BIT);
    flirCam.setRefreshRate(MLX90640_2_HZ);
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }




  //rgb sensor test results.
  lineCount += 10;
  delay(100);
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("RGB");
  if (TCS34725_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }
  //gyro sensor test results.
  lineCount += 10;
  delay(100);
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("GYRO");
  if (MPU6050_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }
  //gas sensor test resuts.
  lineCount += 10;
  delay(100);
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("GAS");
  if (CCS811_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }
  //humidity sensor test results.
  lineCount += 10;
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("HUMIDITY");
  if (HDC1080_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }
  // light sensor test results.
  lineCount += 10;
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("UV");
  if (LIGHTQ_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }
  // MLX90614 test results.
  lineCount += 10;
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("IR");
  if (IR_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }
  // oxi test results.
  lineCount += 10;
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("OXIMETER");
  if (OXI_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }

  delay(5000);
  Wire.setClock(400000);
}

void showSystemStatus() {
  // in here we just get a look at all hardware connected to the unit.
  clearAll();
  showStatus("not ready!");
}
/*
   Main menu system - sets the active menu and displays the corresponding list.
*/
void showMenuSystem(int m) {
  // m selects the buttons to be displayed (0-4)
  int menu_x = 240;
  int menu_y = 27;
  int text_offset = 10;
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_BLACK);
  tft.fillRoundRect(menu_x, 10, 80, 40, 10, ILI9341_BLUE);
  tft.fillRoundRect(menu_x, 55, 80, 40, 10, ILI9341_YELLOW);
  tft.fillRoundRect(menu_x, 100, 80, 40, 10, ILI9341_GREEN);
  tft.fillRoundRect(menu_x, 145, 80, 40, 10, ILI9341_CYAN);
  tft.fillRoundRect(menu_x, 190, 80, 40, 10, ILI9341_RED);
  tft.setCursor(menu_x + text_offset, menu_y);
  tft.print(main[m][0]);     //  button1
  tft.setCursor(menu_x + text_offset, menu_y + 45);
  tft.print(main[m][1]);        //  button2
  tft.setCursor(menu_x + text_offset, menu_y + 90);
  tft.print(main[m][2]);        //  button3
  tft.setCursor(menu_x + text_offset, menu_y + 135);
  tft.print(main[m][3]);        //  button4
  tft.setCursor(menu_x + text_offset, menu_y + 180);
  tft.print(main[m][4]);     //  button5

}

// the idea here is to return with a number 0-9 or a '.' or <OK> to complete the transaction (ENTRY_COMPLETE=true;)
int parseForNumber(int x, int y) {
  int mx = x / 50;
  int my = y / 50;
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(40, 50);
  tft.print("X : " + String(mx));
  tft.print("  Y : " + String(my));
  //TODO  -more parsing...


  int number = 0;
  return number;
}
/*
  Gyro stuff
*/
void showGyroData() {
  sensors_event_t a, g, temp;
  myGyro.getEvent(&a, &g, &temp);
  /* Print out the values */
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(20, 80);
  tft.print("Acceleration:");
  tft.setCursor(20, 100);
  tft.print("X:");
  tft.print(a.acceleration.x);
  tft.print(", Y:");
  tft.print(a.acceleration.y);
  tft.print(", Z:");
  tft.print(a.acceleration.z);
  tft.print("m/s^2    ");

  tft.setCursor( 20, 120);
  tft.print("Rotation:");
  tft.setCursor(20, 140);
  tft.print("X:");
  tft.print(g.gyro.x);
  tft.print(", Y:");
  tft.print(g.gyro.y);
  tft.print(", Z:");
  tft.print(g.gyro.z);
  tft.print(" rad/s   ");

  tft.setCursor( 20, 160);
  tft.print("IMU IC Temp: ");
  tft.print(temp.temperature);
  tft.print(" deg C  ");
}
/*
  here we interrogate the TCS34725 for rgb values 0-255 per colour
*/
void getColourSample() {
  if (TCS34725_READY) {
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
    tft.setCursor(20, 80);
    float red, green, blue;
    rgbSensor.setInterrupt(false);  // turn on LED
    delay(60);  // takes 50ms to read
    rgbSensor.getRGB(&red, &green, &blue);
    rgbSensor.setInterrupt(true);  // turn off LED
    tft.print("R:\t"); tft.print(int(red));
    tft.print("\tG:\t"); tft.print(int(green));
    tft.print("\tB:\t"); tft.print(int(blue));
    tft.setCursor(20, 100);
    tft.print((int)red, HEX); tft.print((int)green, HEX); tft.print((int)blue, HEX);
  } else {
    showStatus("RGB sensor not connected!");
  }
}
/*

*/
void demoBluetooth() {
  showStatus("BLUETOOTH DEMO");
}

/*
  Show MLX 640 stuff  ********************************************
*/

void getFlirStatus() {
  if (FLIR_READY) {
    showStatus("FLIR STATUS");
    tft.setCursor(20, 60);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
    tft.print("Current mode : ");
    if (flirCam.getMode() == MLX90640_CHESS) {
      //tft.setCursor(140, 60);
      tft.print("Chess");
    } else {
      tft.print("Interleave");
    }
    tft.setCursor(20, 80);
    tft.print("Current Resolution : ");
    //tft.setCursor(160, 80);
    mlx90640_resolution_t res = flirCam.getResolution();
    switch (res) {
      case MLX90640_ADC_16BIT: tft.print("16 bit"); break;
      case MLX90640_ADC_17BIT: tft.print("17 bit"); break;
      case MLX90640_ADC_18BIT: tft.print("18 bit"); break;
      case MLX90640_ADC_19BIT: tft.print("19 bit"); break;
    }
    flirCam.setRefreshRate(MLX90640_16_HZ);
    tft.setCursor(20, 100);
    tft.print("Current frame rate: ");
    //tft.setCursor(140, 100);
    mlx90640_refreshrate_t rate = flirCam.getRefreshRate();
    switch (rate) {
      case MLX90640_0_5_HZ: tft.print("0.5 Hz"); break;
      case MLX90640_1_HZ: tft.print("1 Hz"); break;
      case MLX90640_2_HZ: tft.print("2 Hz"); break;
      case MLX90640_4_HZ: tft.print("4 Hz"); break;
      case MLX90640_8_HZ: tft.print("8 Hz"); break;
      case MLX90640_16_HZ: tft.print("16 Hz"); break;
      case MLX90640_32_HZ: tft.print("32 Hz"); break;
      case MLX90640_64_HZ: tft.print("64 Hz"); break;
    }
  } else {
    showStatus("Flir - offline!");
  }
}
//
void toggleFlirMode() {
  if (FLIR_READY) {
    if (flirCam.getMode() == MLX90640_CHESS) {
      flirCam.setMode(MLX90640_INTERLEAVED);
    }
    if (flirCam.getMode() == MLX90640_INTERLEAVED) {
      flirCam.setMode(MLX90640_CHESS);
    }
  } else {
    showStatus("Flir - offline!");
  }
}
//
void setFlirResolution() {
  if (FLIR_READY) {
    showStatus("SET RESOLUTION");
    flirCam.setResolution(MLX90640_ADC_18BIT);
  } else {
    showStatus("Flir - offline!");
  }
}
void toggleFlirOnOff() {
  if (SystemMode == FLIR) {
    showStatus("Flir OFF");
    SystemMode = IDLE;//idle
  } else {
    showStatus("Flir ON");
    SystemMode = FLIR;//idle
  }
}
/*
   Because the adafruit library only returns numbers, we will take the values and
   plot them to the screen at a default scale and pallete. This is adjustable with
   functions setZoom() and setFlirColours will set the optional parameters.
*/
uint16_t numberToColour(float t) {
  // t is your temp range (20-39)
  // here is where we map the temperature to a selected pixel from a group of custom palettes.
  if (t < 20) return ILI9341_BLACK;
  else if (t < 23) return ILI9341_BLUE;
  else if (t < 25) return ILI9341_MAGENTA;
  else if (t < 27) return ILI9341_CYAN;
  else if (t < 29) return ILI9341_GREEN;
  else if (t < 31) return ILI9341_YELLOW;
  else if (t < 33) return ILI9341_ORANGE;
  else if (t < 35) return ILI9341_PINK;
  else if (t < 37) return ILI9341_RED;
  else if (t < 39) return ILI9341_BLACK;
}

void bufferToScreen() {
  uint8_t x = 200 / GFIPZ;
  uint8_t y = 180 / GFIPZ;
  for (uint8_t h = 0; h < 24; h++)
  {
    for (uint8_t w = 0; w < 32; w++)
    {
      float t = flirFrame[h * 32 + w];
      //centering position offset depending on zoom factor 1 - 4

      int xpixel = x + (w * GFIPZ);
      int ypixel = y + (h * GFIPZ);
      //tft.fillRect(xpixel, ypixel, (xpixel + GFIPZ), (ypixel + GFIPZ), numberToColour(t));

      //
      tft.drawPixel(xpixel, ypixel,  numberToColour(t));
      tft.drawPixel(xpixel + 1, ypixel,  numberToColour(t));
      tft.drawPixel(xpixel, ypixel + 1,  numberToColour(t));
      tft.drawPixel(xpixel + 1, ypixel + 1,  numberToColour(t));
    }
  }
}

void updateFlirImage() {
  if (FLIR_READY) {
    //clearAll();
    //showStatus("THERMAL CAMERA");
    //TODO: cycle through the frame buffer and create an image on the tft
    if (flirCam.getFrame(flirFrame) != 0) {
      showStatus("Frame retrieve fail");
      return;
    }
    showStatus("Frames OK.");
    bufferToScreen();
  } else {
    showStatus("Flir - gone offline!");
  }
}
void setZoom(int zv) {
  //TODO: to be implemented later
  if (zv = 0) {
    GFIPZ = 1;
    showStatus("Zoom set to 1:1");
  }
  if (zv = 1) {
    GFIPZ = 2;
    showStatus("Zoom set to 2:1");
  }
  if (zv = 2) {
    GFIPZ = 3;
    showStatus("Zoom set to 3:1");
  }
  if (zv = 3) {
    GFIPZ = 4;
    showStatus("Zoom set to 4:1");
  }
}
void setFlirColours() {
  //TODO: to be implemented later
}
/*
   Show MLX90614 IR Results
*/
void ShowIRResults() {
  if (IR_READY) {
    showStatus("IR Body Temp:");
    Wire.setClock(400000);
    ClearScreen();         //Clear the display
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(35, 5);
    tft.println("Body Temp:");
    tft.setTextSize(2);
    tft.setCursor(20, 23);
    tft.print(OTC);
    tft.print((char)247);
    tft.println("C");
    if (OTC > 37.8) {
      tft.setTextSize(1);
      tft.setCursor(32, 45);
      tft.println("FAIL - Fever!");
    }
    if (OTC < 36.1) {
      tft.setTextSize(1);
      tft.setCursor(0, 45);
      tft.println("Ready - Place Sensor!");
    }
    if (OTC < 37.8 & OTC > 36.1) {
      tft.setTextSize(1);
      tft.setCursor(32, 45);
      tft.println("PASS - Normal");
    }

  } else {
    showStatus("No IR");
  }
}
//  Read the IR sensor
void ReadIRTemp() {
  if ( IR_READY ) {
    Wire.setClock(100000);
    OTC = mlx.readObjectTempC();
    OTF = mlx.readObjectTempF();
    ATC = mlx.readAmbientTempC();
    ATF = mlx.readAmbientTempF();
    Wire.setClock(400000);
    ShowIRResults();
  } else {
    showStatus("No IR");
  }
}
/*
   OxiTest - Here we wait for the finger and when we have a reading we will display the results.
   Reading the IR value it will permit us to know if there's a finger on the sensor or not. Also
   detecting a heartbeat. If a finger is detected, clear the display,draw the first bmp
   picture (little heart). Near it display the average BPM you can display the BPM if you want.
   If a heart beat is detected, clear the display and draw the second picture (bigger heart).
   And still display the average BPM
*/
void oxiTest() {
  if (OXI_READY) {
    long irValue = particleSensor.getIR();
    if (irValue > 7000) {
      clearBody();
      tft.drawBitmap(5, 5, logo2_bmp, 24, 21, ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(50, 0);
      tft.println("BPM");
      tft.setCursor(50, 18);
      tft.println(beatAvg);

      if (checkForBeat(irValue) == true)
      {
        clearBody();
        tft.drawBitmap(0, 0, logo3_bmp, 32, 32, ILI9341_WHITE);
        tft.setTextSize(2);
        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(50, 0);
        tft.println("BPM");
        tft.setCursor(50, 18);
        tft.println(beatAvg);
        //tone(3,1000);                                   //And tone the buzzer for a 100ms you can reduce it it will be better
        delay(100);
        //noTone(3);                                      //Deactivate the buzzer to have the effect of a "bip"
        //We sensed a beat!
        long delta = millis() - lastBeat;                 //Measure duration between two beats
        lastBeat = millis();
        beatsPerMinute = 60 / (delta / 1000.0);           //Calculating the BPM
        if (beatsPerMinute < 255 && beatsPerMinute > 20)  //To calculate the average we strore some values (4) then do some math to calculate the average
        {
          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
          rateSpot %= RATE_SIZE; //Wrap variable
          //Take average of readings
          beatAvg = 0;
          for (byte x = 0 ; x < RATE_SIZE ; x++)
            beatAvg += rates[x];
          beatAvg /= RATE_SIZE;
        }
      }

    }
    //  If no finger is detected it inform the user and put the average BPM to 0 or
    //  it will be stored for the next measure.

    if (irValue < 7000) {
      beatAvg = 0;
      clearBody();
      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(30, 5);
      tft.print("Please place your finger");
      tft.setCursor(30, 15);
      tft.print("on the sensor");
      //noTone(3);
    }
  } else {
    showStatus("No Oximeter attached!");
  }
}
void showWeather() {
  if (BME280_READY) {
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.setCursor(20, 50);
    tft.print("Temp = ");
    tft.print(myBME280.readTempC());
    tft.println(" C");
    tft.setCursor(20, 70);
    tft.print("Humidity = ");
    tft.print(myBME280.readFloatHumidity());
    tft.println(" %");
    tft.setCursor(20, 90);
    tft.print("Baro = ");
    tft.print(myBME280.readFloatPressure() * 0.00750062, 2);
    tft.println(" hPa");
    tft.setCursor(20, 110);
    tft.print("Dewpoint = ");
    tft.print(myBME280.dewPointC() , 2);
    tft.println(" C");
    tft.setCursor(20, 130);
    tft.print("Alt. = ");
    tft.print(myBME280.readFloatAltitudeMeters(), 2);
    tft.println(" meters");
  }
  if (CCS811_READY) {
    tft.setCursor(20, 150);
    tft.print("CO2 = ");
    tft.print(myCCS811.getCO2());
    tft.println(" ppm");
    tft.setCursor(20, 170);
    tft.print("TVOC = ");
    tft.print(myCCS811.getTVOC());
    tft.println(" ppb");
  }
}
void showLightData() {
  if (LIGHTQ_READY) {
    showStatus("Light Quality");
    clearBody();
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.setCursor(40, 40);
    tft.setTextSize(1);
    float UVindex = light_data.readUV();
    // the index is multiplied by 100 so to get the
    // integer index, divide by 100!
    UVindex /= 100.0;
    tft.print("Vis: ");
    tft.println(light_data.readVisible());
    tft.setCursor(40, 60);
    tft.print("UV Index: ");
    tft.println(UVindex);
    tft.setCursor(40, 80);
    tft.print("Ir: ");
    tft.println(light_data.readIR());
  }
}
void showRGBReader() {

}
void showWifi() {
  showStatus("Wifi Page");
  clearBody();
  //WiFi.mode(WIFI_ON);
  String ip = WiFi.localIP().toString();
  //todo test for connection
  if (WiFi.status() == WL_CONNECTED) {
    showStatus("Wifi connected");
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(15, 70);
    tft.print("IP Address: ");
    tft.println(ip);
    tft.setCursor(15, 90);
    tft.print("Connected to: ");
    tft.println(WiFi.SSID());
  } else {
    showStatus("Wifi not connected");
  }
}

void scanForNetworks() {
  int lineStart = 40;
  int row = 70;
  showStatus("Scan in progress");
  // WiFi.scanNetworks will return the number of networks found
  //WiFi.mode(WIFI_ON);
  int n = WiFi.scanNetworks();
  showStatus("Scan complete");
  if (n == 0) {
    showStatus("No networks were found");
  } else {
    tft.setCursor(lineStart, 50);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
    tft.print( "Networks found - ");
    tft.setCursor(150, 50);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print(n);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    if (n > 14) {
      for (int i = 0; i < 14; ++i) {
        // Print the SSID and RSSI for each network found
        tft.setCursor(lineStart, row);
        tft.setTextColor(ILI9341_MAGENTA, ILI9341_BLACK);
        tft.print(WiFi.SSID(i));
        tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
        tft.print(" (");
        tft.print(WiFi.RSSI(i));
        tft.print("db)");
        tft.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.setCursor(lineStart + 160, row);
        tft.print(i + 1);
        delay(10);
        row += 10;
      }
      delay(4000);
      clearBody();
      int lineStart = 40;
      int row = 70;
      for (int i = 14; i < n; ++i) {
        // Print the SSID and RSSI for each network found
        tft.setCursor(lineStart, row);
        tft.setTextColor(ILI9341_MAGENTA, ILI9341_BLACK);
        tft.print(WiFi.SSID(i));
        tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
        tft.print(" (");
        tft.print(WiFi.RSSI(i));
        tft.print("db)");
        tft.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.setCursor(lineStart + 160, row);
        tft.print(i + 1);
        delay(10);
        row += 10;
      }

    }
    if (n < 15) {
      for (int i = 0; i < n; ++i) {
        // Print the SSID and RSSI for each network found
        tft.setCursor(lineStart, row);
        tft.setTextColor(ILI9341_MAGENTA, ILI9341_BLACK);
        tft.print(WiFi.SSID(i));
        tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
        tft.print(" (");
        tft.print(WiFi.RSSI(i));
        tft.print("db)");
        tft.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.setCursor(lineStart + 160, row);
        tft.print(i + 1);
        delay(10);
        row += 10;
      }
    }
  }
}

void selectNetwork() {
  clearStatus();
  clearBody();
  int val = getKeyboardNumeric();
}
void connectToNetwork(int n) {

}
void reconnectToNetwork() {
  clearStatus();
  clearBody();
  PAD_DOWN = false;
  WiFi.reconnect();
  tft.setTextSize(1);
  showStatus("Connecting to network...!");
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(40, 50);
  while ( WiFi.status() != WL_CONNECTED ) {
    tft.setCursor(220, 15);
    tft.print("+");
    delay(200);
    tft.setCursor(220, 15);
    tft.print("x");
    delay ( 150 );
    if (PAD_DOWN)break;
  }
  if (WiFi.status() == WL_CONNECTED) {
    showStatus("Connected!");
    tft.setCursor(35, 50);
    tft.print("Your network: ");
    tft.print(WiFi.SSID());
    tft.setCursor(35, 70);
    tft.print("Your IP address: ");
    tft.print(WiFi.localIP());
  } else  showStatus("Not Connected!");

}
void disconnectFromNetwork() {
  WiFi.disconnect();
  showStatus("Dropped network connection");
}


void showThermal() {
  clearAll();
  showStatus("FLIR Camera comming online");
  SystemMode = FLIR;
}
void showAdmin() {
  clearBody();
  tft.setCursor(40, 80);
  showStatus("No Administrator!");
}
void showMedical() {
  clearBody();
  showStatus("Medical Assistance...");
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(40, 80);
  tft.print("Select functions from menu.");
  MENU_MODE = M_MEDICAL;
  showMenuSystem(MENU_MODE);
}
void showMail() {
  showStatus("Mail Server:");
  clearBody();
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(40, 80);
  tft.print("no new mail has arrived");
}
void showNews() {
  clearBody();
  showStatus("Breaking News...");
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(15, 60);
  tft.print("Top story today, this coming out of");
  tft.setCursor(15, 70);
  tft.print("Washington, former president sends");
  tft.setCursor(15, 80);
  tft.print("angry mob to capital hill, riots");
  tft.setCursor(15, 90);
  tft.print("ensue, see live coverage, on CNN.");
}
void showWorldNews() {
  showStatus("World News Today...");
  clearBody();
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(15, 50);
  tft.print("Three years later and COVID-19");
  tft.setCursor(15, 60);
  tft.print("still plagues the planet");
  tft.setCursor(15, 70);
  tft.print("See live coverage,on CNN.");
  delay(1000);
}
void showSports() {
  showStatus("Sports Stories Today...");
  clearBody();
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(15, 60);
  tft.print("Top story today, NFL Superbowl");
  tft.setCursor(15, 70);
  tft.print("coverage starts soon");
  tft.setCursor(15, 80);
  tft.print("See live coverage,on CNN");
}
void showTechNews() {
  clearBody();
  showStatus("Tech Stories Today...");
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(15, 60);
  tft.print("Top story today, CapSoft");
  tft.setCursor(15, 70);
  tft.print("buys Microsoft for one dollar");
  tft.setCursor(15, 80);
  tft.print("See live coverage,on CNN");
}
void showLifestyles() {
  clearBody();
  showStatus("Lifestyles Today...");
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(15, 40);
  tft.print("Is Yogurt good for you?");
  tft.setCursor(15, 60);
  tft.print("We ask the experts...");
  tft.setCursor(15, 80);
  tft.print("Tonight on CNN.");
  tft.setCursor(15, 100);
  tft.print("Politics, who cares!");
  tft.setCursor(15, 120);
  tft.print("with Wolf Blitzer,later on CNN.");
  tft.setCursor(15, 140);
  tft.print("Cooking with Chris Cuomo....");
  tft.setCursor(15, 150);
  tft.print("later on CNN.");
  tft.setCursor(15, 180);
  tft.print("Chris cooks up some soup...,");
  tft.setCursor(15, 190);
  tft.print("much later on CNN.");
}

/*
   This is where all the action happens, this is the OS,
     here we can look at the x and y values and respond
     there are five buttons down the right side of the screen
     the X coordinates will be 60 to 77.
     the Y variables are as follows...
     B1 - 10 to 18
     B2 - 21 to 32
     B3 - 35 to 45
     B4 - 48 to 59
     B5 - 62 to 73
     So far there are 5 menus to manage -> 0 - 4
     @PARAM x current x value
     @PARAM y current y value

   Command interpeter - derived from MENU_MODE/BUTTON_NUMBER matrix, calls the necessary action(s).
*/
int parseMenuTouch(int x, int y) {
  digitalWrite(2, HIGH);
  int mx = x / 50;
  int my = y / 50;
  if (DEBUG) {
    // direct print to status bar
    tft.setCursor(10, 180);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
    tft.print("X=" + x); tft.print("Y=" + y);
  }
  //menu buttons are active
  if (GLOBAL_TOUCH_TYPE == MENU) {
    if (mx < 60) {
      return  0;
    }
    // isolate the rows(buttons)
    // changed 9 to 5 (2/15/21) to increase top sensitivity
    if (my > 5 && my < 18) {
      return 1;
    }
    if (my > 20 && my < 32) {
      return  2;
    }
    if (my > 34 && my < 46) {
      return  3;
    }
    if (my > 48 && my < 60) {
      return  4;
    }
    if (my > 61 && my < 74) {
      return  5;
    }
  }

}
/*
   in the main loop action is called if PAD_DOWN = true
   The next thing to do here is to clean up this code by using  arrays to
   control the actions.

  your  available menus

  const int M_M1 = 0;
  const int M_M2 = 1;
  const int M_M3 = 2;
  const int M_FLIR = 3;
  const int M_FLIR2 = 4;
  const int M_FLIR_ZOOM = 5;
  const int M_FLIR_COLOUR = 6;
  const int M_TIME = 7;
  const int M_NETWORK = 8;
  const int M_NEWS = 9;
  const int M_MEDICAL = 10;
  const int M_SERVICES = 11;

        0   main 1    (m) weather, time, flir, news, more.
        1   main 2    (m) network, email, medical, admin.
        2   main 3    (b) bluetooth, GYRO, RGB colour sensor,hardware services.
        3   flir 1    (m) flir first options menu.
        4   flir 2    (n) flir next options menu.
        5   flir 3    (b) flir zoom.
        6   flir 4    (b) flir colours
        7   time 1    (b) get time....
        8   network 1 (b) get network...
        9   news 1    (b) get news...
        10  medical 1 (b) get medical...
        11  services 1 (b)system stuff...



*/
void goBackToTheFuture() {
  clearAll();
  MENU_MODE = M_M1;
  SystemMode = IDLE;
  showMenuSystem(MENU_MODE);
}

void action(int bp) {
  digitalWrite(2, LOW);
  if (DEBUG) {
    tft.setCursor( 15, 15 );
    tft.print(bp);
  }
  // the first menu list. weather, time,...
  if (MENU_MODE == M_M1) {
    //  main menu, the weather.
    if (bp == 1) {
      clearAll();
      showStatus("Current Weather");
      SystemMode = ENVIRONMENT;
      return;
    }
    // time functions, sorry no time travel yet, just the time!
    if (bp == 2) {
      clearAll();
      MENU_MODE = M_TIME;
      SystemMode = TIME;
      showMenuSystem(MENU_MODE);
      showDateTime();
      return;
    }
    // show thermal imager
    if (bp == 3) {
      clearAll();
      MENU_MODE = M_FLIR;
      showMenuSystem(MENU_MODE);
      showThermal();
      return;
    }
    //news
    if (bp == 4) {
      clearAll();
      MENU_MODE = M_NEWS;
      showMenuSystem(MENU_MODE);
      SystemMode = IDLE;
      showNews();
      return;
    }
    // show next menu items (more)
    if (bp == 5) {
      //clearAll();
      MENU_MODE = M_M2;
      showMenuSystem(MENU_MODE);
      return;
    }
  }
  // second menu list.network, email, etc.
  if (MENU_MODE == M_M2) {
    //main menu part-2 - the first more menu.
    //the network menu.
    if (bp == 1) {
      //clearAll();
      MENU_MODE = M_NETWORK;
      SystemMode = IDLE;
      showMenuSystem(MENU_MODE);
      showWifi();
      return;
    }
    //mail.
    if (bp == 2) {
      SystemMode = IDLE;
      showMail();
      return;
    }
    //medical sub-menu.
    if (bp == 3) {
      SystemMode = IDLE;
      showMedical();
      return;
    }
    //admin page selected.
    if (bp == 4) {
      SystemMode = IDLE;
      showAdmin();
      return;
    }
    if (bp == 5) {
      //SystemMode = IDLE;
      MENU_MODE = M_M3;
      showMenuSystem(MENU_MODE);
      return;
    }
  }
  // third menu list.
  if (MENU_MODE == M_M3) {

    if (bp ==  1) {
      demoBluetooth();
      return;
    }
    if (bp ==  2) {
      SystemMode = GYRO;
      showStatus("GYRO DATA");
      clearBody();
      return;
    }
    if (bp ==  3) {
      SystemMode = RGB_SCAN;
      getColourSample();
      return;
    }
    if (bp ==  4) {
      MENU_MODE = M_SERVICES;
      showMenuSystem(MENU_MODE);
      showSystemStatus();
      return;
    }
    if (bp ==  5) {
      goBackToTheFuture();
      return;
    }
  }
  //flir sub menu 1
  if (MENU_MODE == M_FLIR) {
    if (bp ==  1) {
      toggleFlirMode();
      return;
    }
    if (bp ==  2) {
      setFlirResolution();
      return;
    }
    if (bp ==  3) {
      //status
      getFlirStatus();
      return;
    }
    if (bp ==  4) {
      toggleFlirOnOff();
      return;
    }
    if (bp ==  5) {
      MENU_MODE = M_FLIR2;
      showMenuSystem(MENU_MODE);
      return;
    }
  }
  // flir 2 zoom and colour.
  if (MENU_MODE == M_FLIR2) {
    if (bp == 1) {
      MENU_MODE = M_FLIR_ZOOM;
      showMenuSystem(MENU_MODE);
      return;
    }
    if (bp == 2) {
      MENU_MODE = M_FLIR_COLOUR;
      showMenuSystem(MENU_MODE);
      return;
    }
    if (bp == 5) {
      goBackToTheFuture();
      return;
    }
  }
  //flir sub-menu zoom
  if (MENU_MODE == M_FLIR_ZOOM) {
    if (bp == 1) {

      setZoom(0);
      return;
    }
    if (bp == 2) {

      setZoom(1);
      return;
    }
    if (bp == 3) {

      setZoom(2);
      return;
    }
    if (bp == 4) {

      setZoom(3);
      return;
    }
    //back to main menu
    if (bp == 5) {
      MENU_MODE = M_FLIR;
      showMenuSystem(MENU_MODE);
      return;
    }
  }
  //sub-menu time
  if (MENU_MODE == M_TIME) {
    if (bp == 1) {
      syncronizeRTC();
      return;
    }
    if (bp == 2) {
      //TODO: setAlarm();
      return;
    }
    if (bp == 3) {
      //TODO: clearAlarms();
      return;
    }
    if (bp == 4) {
      //TODO: alarms info
      return;
    }
    if (bp == 5) {
      goBackToTheFuture();
      return;
    }
  }
  //sub-menu network
  if (MENU_MODE == M_NETWORK) {
    if (bp == 1) {
      reconnectToNetwork();
      return;
    }
    if (bp == 2) {
      disconnectFromNetwork();
      return;
    }
    if (bp == 3) {
      selectNetwork();
      return;
    }
    if (bp == 4) {
      scanForNetworks();
      return;
    }
    //back to main menu
    if (bp == 5) {
      goBackToTheFuture();
      return;
    }
  }
  //news sub-menu
  if (MENU_MODE == M_NEWS) {

    if (bp == 1) {
      showSports();
      return;
    }
    if (bp == 2) {
      showLifestyles();
      return;
    }
    if (bp == 3) {
      showTechNews();
      return;
    }
    if (bp == 4) {
      showWorldNews();
      return;
    }
    //back to main menu
    if (bp == 5) {
      goBackToTheFuture();
      return;
    }
  }
  //medical sub-menu
  if (MENU_MODE == M_MEDICAL) {
    if (bp == 1) {
      //oxi testing
      oxiTest();
      return;
    }
    if (bp == 2) {
      //ir testing
      return;
    }
    if (bp == 3) {
      //sp testing
      return;
    }
    if (bp == 4) {
      //tba
      return;
    }
    //back to main menu
    if (bp == 5) {
      goBackToTheFuture();
      return;
    }
  }
  //services menu.
  if (MENU_MODE == M_SERVICES) {
    if (bp == 1) {
      //show all connected devices
      showStatus("Hardware Scan:");
      scanI2CBus();
      initializeSensorGroup();
      return;
    }
    if (bp == 2) {
      showStatus("Disable Hardware");
      deactivateAll();
      initializeSensorGroup();
      return;
    }
    if (bp == 3) {
      showStatus("Re-enable hardware");
      scanI2CBus();
      initializeSensorGroup();
      return;
    }
    if (bp == 4) {
      showStatus("      -does nothing-");
      //tba
      return;
    }
    //back to main menu
    if (bp == 5) {
      goBackToTheFuture();
      return;
    }
  }
  //next....
}





#endif