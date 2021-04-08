/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

#include <ESP8266WiFi.h>

/*
#ifndef STASSID
#define STASSID "0024A5B58F84"
#define STAPSK  "fncu4tredt6rf"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
*/

int Counter = 0;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

/*
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
*/
//  if (MDNS.begin("esp8266")) {
//    Serial.println("MDNS responder started");
//  }

}

// the loop function runs over and over again forever
void loop() {
  Serial.print("Count:");
  Serial.println(Counter);
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  delay(100);                      // Wait for 0.5 second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(900);                      // Wait for  seconds (to demonstrate the active low LED)
  ++Counter;
}
