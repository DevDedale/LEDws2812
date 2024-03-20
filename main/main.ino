

////////////////////////////////////////
//// Choix de la machine
////////////////////////////////////////


// specific Mehdi
//Board type : NodeMCU 1.0 ESP12E

// specific Olivier
//Board type : Arduin Nano
// choisir dans le menu : "Processor: ATmega328P (Old BootLoader)""

// Which pin on the Arduino is connected to the NeoPixels?
// specific Mehdi 3 au lieu de 6
#define PIN 3  // On Trinket or Gemma, suggest changing this to 1
//#define PIN 9  // For Arduino


////////////////////////////
//// Options de l'application
////////////////////////////


#define USE_SERIAL_PRINT 0
#define USE_MQTT 1
#define USE_MQTT_DEBUG 0
#define SHOW_MAC_ADDRESS 0
#define DELAYVAL 0  // Time (in milliseconds) to pause between loop
#define SALLE_MAGIE 1
#define SALLE_MINE 0
#define USE_LED 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 300  // Popular NeoPixel ring size


////////////////////////////
//// Gestion Communication MQTT
////////////////////////////
// on utilise la macAdress
// pour l'identification de la led
#if USE_MQTT==1
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

////////////////////////////
// to send MQTT messages to server
#include "EspMQTTClient.h"

#define MQTT_DEVICENAME "LED2" //obsolete on utilise l'addresse mac désormais
#define MQTT_REALM "dedale"

EspMQTTClient client(
  "mgi-dedale",
  "xi6Hoogh",
  "10.65.0.200",  // MQTT Broker server ip
  "",   // Can be omitted if not needed
  "",   // Can be omitted if not needed
  "ClientLED1"     // Client name that uniquely identify your device
  );

String nameLED;

char WillMessage[255];
String GetTopic(String topic) {
  return (String(MQTT_REALM) + "/devices/" + nameLED + "/" + topic);
  //return (String(MQTT_REALM) + "/devices/" + MQTT_DEVICENAME + "/" + topic);
}
////////////////////////////

#endif

////////////////////////////



////////////////////////////
// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library
////////////////////////////
// gestion des LEDS

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif





// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int whichAnimation = 1;  //0=Black 1=Corruption 2=Blue
long lastMilli = 0;
long nowMilli = 0;






void OnMessageSet(const String& payload) {
  if (payload == "black" || payload == "\"black\"") {
    whichAnimation = 0;
    SetAllPixelsColor(0, 0, 0);
  }
  if (payload == "red" || payload == "\"red\"") {
    whichAnimation = 1;
    SetAllPixelsColor(255, 0, 0);
  }
  if (payload == "blue" || payload == "\"blue\"") {
    whichAnimation = 2;
    SetAllPixelsColor(0, 0, 255);
  }

  //client.publish(GetTopic("ws2812Color").c_str(), payload);
}


#if USE_MQTT==1
// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished() {
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(GetTopic("animation/set").c_str(), OnMessageSet);

  ///////
  client.publish(GetTopic("status/connected").c_str(), "1", true);  //true is for retain (durée de vie des messages MQTT)                                                 // Set all pixel colors to 'off'
}
#endif

///////////////////////////////////
//// Colors    ////////////////////
//// Animation ////////////////////
void DoAnimation(int which) {
  if (which == 1)
  {
  #if SALLE_MAGIE==1
    DoAnimationCorruptionMagie();
  #else
    DoAnimationCorruptionMine();
  #endif
  }
}

void DoAnimationCorruptionMagie() {
  unsigned long t0 = millis();
  long t;
  for (int i = 0; i < NUMPIXELS; i+=3) {
    t = t0 + 200*i;
    int r = 150.0f + 50.0f*sin((float) (i*0.04f + t * 0.001f)*3) + 50.0f*sin((float)  (i*0.03f + t * 0.002f)*7);
    int g = 4.0f + 2.0f*sin((float) t * 0.004f) + 2.0f*sin((float) t * 0.009f);
    pixels.setPixelColor(i, pixels.Color(r, g, 0));
  }
  pixels.show();
}

void DoAnimationCorruptionMine() {
  float t = (float) millis();
  for (int i = 0; i < NUMPIXELS; i+=6) {
    float ifl = (float) i;
    float s = sin((float) (ifl*0.1f) + t*0.0006f);
    s = s*s;
    s = s*s;
    float s1 = sin((float) (ifl*0.15f) + t*0.001f);
    s1 = s1*s1;
    s1 = s1*s1;
    int r = 10.0f + 120.0f*(s + s1);
    float s2 = (sin((float) (ifl*0.17f) + t*0.002f) * sin((float) (ifl*0.11f) + t*0.0013f));
    s2 = s2*s2;
    int g = 5.0f + ((s + s1) * s2)*50.0f;
    pixels.setPixelColor(i, pixels.Color(r, g, 0));
    pixels.setPixelColor(i+1, pixels.Color(r, g, 0));
    pixels.setPixelColor(i+2, pixels.Color(r, g, 0));
    pixels.setPixelColor(i+3, pixels.Color(r, g, 0));
    pixels.setPixelColor(i+4, pixels.Color(r, g, 0));
    pixels.setPixelColor(i+5, pixels.Color(r, g, 0));
  }
  pixels.show();
}

void SetAllPixelsColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}


//////////////////////////////////
/////// setup and loop ///////////
//////////////////////////////////
void setup() {
  #if USE_SERIAL_PRINT==1 
  Serial.begin(115200);
  #endif

  delay(2);

  lastMilli = millis();

  ///// Choix du nom pour MQTT //////
  #if USE_MQTT == 1

  #if USE_MQTT_DEBUG == 1
  client.enableDebuggingMessages(); 
  #endif

  String macAddress = WiFi.macAddress();
  nameLED = "LED"+ macAddress.substring(12,14) + macAddress.substring(15,17);

  #if SHOW_MAC_ADDRESS==1
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  #endif

  //bool result = client.setMaxPacketSize(256);
  //client.enableMQTTPersistence();
  client.setMqttReconnectionAttemptDelay( 1000);  

  // specific Mehdi
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  GetTopic("status/connected").toCharArray(WillMessage, 255);
  client.enableLastWillMessage(WillMessage, "0", true);

  #endif



  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  
#if USE_LED==1
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  SetAllPixelsColor(250, 0, 0);
#endif
}


void loop() {
  #if USE_MQTT == 1
    client.loop();
  #endif

  #if USE_LED==1
  nowMilli = millis();
  if(nowMilli - lastMilli > 5)
  {
    DoAnimation(whichAnimation);
    lastMilli = nowMilli;
    //Serial.println(nowMilli);
  }
  #endif

  if(DELAYVAL>0)
    delay(DELAYVAL);

  #if SHOW_MAC_ADDRESS==1
  {
   Serial.println(WiFi.macAddress());
   Serial.println(nameLED);
  }
  #endif
}

//ESP Board MAC Address:  EC:FA:BC:2F:ED:FC
//ESP Board MAC Address:  EC:FA:BC:2F:EF:64
//ESP Board MAC Address:  EC:FA:BC:2F:EE:A9
//ESP Board MAC Address:  EC:FA:BC:2F:F3:E0
//ESP Board MAC Address:  EC:FA:BC:2F:F1:93
//ESP Board MAC Address:  EC:FA:BC:2F:EF:5C
//ESP Board MAC Address:  EC:FA:BC:2F:F4:35

