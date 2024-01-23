
// specific Mehdi
//Board type : NodeMCU 1.0 ESP12E


// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library


////////////////////////////
// on utilise la macAdress
// pour l'identification de la led
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

String nameLED;
////////////////////////////


////////////////////////////
// to send MQTT messages to server
#include "EspMQTTClient.h"

#define MQTT_DEVICENAME "LED2" //obsolete on utilise l'addresse mac désormais
#define MQTT_REALM "dedale"

EspMQTTClient client(
  "mgi-dedale",
  "xi6Hoogh",
  "10.65.0.200"  // MQTT Broker server ip
);

char WillMessage[255];
String GetTopic(String topic) {
  return (String(MQTT_REALM) + "/devices/" + nameLED + "/" + topic);
  //return (String(MQTT_REALM) + "/devices/" + MQTT_DEVICENAME + "/" + topic);
}
////////////////////////////



////////////////////////////
// gestion des LEDS

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif


// Which pin on the Arduino is connected to the NeoPixels?
// specific Mehdi 3 au lieu de 6
#define PIN 3  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 300  // Popular NeoPixel ring size


// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 5  // Time (in milliseconds) to pause between loop (desactivated)

int whichAnimation = 0;  //0=Black 1=Corruption 2=Blue






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

  client.publish(GetTopic("OK set").c_str(), payload);
}

void OnMessagefromGenZ(const String& payload) {
  client.publish(GetTopic("Wesh").c_str(), payload);
}


// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished() {
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(GetTopic("animation/set").c_str(), OnMessageSet);
  client.subscribe(GetTopic("leds/genZ").c_str(), OnMessagefromGenZ);

  ///////
  client.publish(GetTopic("status/connected").c_str(), "1", true);  //true is for retain (durée de vie des messages MQTT)                                                 // Set all pixel colors to 'off'
}

///////////////////////////////////
//// Colors    ////////////////////
//// Animation ////////////////////
void DoAnimation(int which) {
  if (which == 1)
    DoAnimationCorruption();
}


void DoAnimationCorruption() {
  unsigned long t0 = millis();
  int t;
  for (int i = 0; i < NUMPIXELS; i+=3) {
    t = t0 + 200*i;
    // int r = 150.0f + 50.0f*sin((float) t * 0.003f) + 50.0f*sin((float) t * 0.007f);
    // int g = 10.0f + 5.0f*sin((float) t * 0.004f) + 5.0f*sin((float) t * 0.009f);
    int r = 150.0f + 50.0f*sin((float) (i*0.04f + t * 0.001f)*3) + 50.0f*sin((float)  (i*0.03f + t * 0.002f)*7);
    int g = 4.0f + 2.0f*sin((float) t * 0.004f) + 2.0f*sin((float) t * 0.009f);
    pixels.setPixelColor(i, pixels.Color(r, g, 0));
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
  Serial.begin(115200);

  ///// Choix du nom pour MQTT //////
  Serial.print("ESP Board MAC Address:  ");
  String macAddress = WiFi.macAddress();
  Serial.println(WiFi.macAddress());
  nameLED = "LED"+ macAddress.substring(12,14) + macAddress.substring(15,17);


  // specific Mehdi
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  GetTopic("status/connected").toCharArray(WillMessage, 255);
  client.enableLastWillMessage(WillMessage, "0", true);
 

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  SetAllPixelsColor(0, 0, 0);
}


void loop() {
  client.loop();

  DoAnimation(whichAnimation);

  //delay(DELAYVAL);
  if(1)
  {
   Serial.println(WiFi.macAddress());
   Serial.println(nameLED);
  }
}

//ESP Board MAC Address:  EC:FA:BC:2F:ED:FC
//ESP Board MAC Address:  EC:FA:BC:2F:EF:64
//ESP Board MAC Address:  EC:FA:BC:2F:EE:A9
//ESP Board MAC Address:  EC:FA:BC:2F:F3:E0
//ESP Board MAC Address:  EC:FA:BC:2F:F1:93
//ESP Board MAC Address:  EC:FA:BC:2F:EF:5C

