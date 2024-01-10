// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif


#include "EspMQTTClient.h"

EspMQTTClient client(
  "mgi-dedale",
  "xi6Hoogh",
  "10.65.0.200"  // MQTT Broker server ip
);

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif


// specific Mehdi
//Board type : NodeMCU 1.0 ESP12E

// Which pin on the Arduino is connected to the NeoPixels?
// specific Mehdi 3 au lieu de 6
#define PIN 3  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 300  // Popular NeoPixel ring size
#define MQTT_DEVICENAME "LED2"
#define MQTT_REALM "dedale"

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 5  // Time (in milliseconds) to pause between pixels

int whichAnimation = 1;  //0=Black 1=Corruption 2=



String GetTopic(String topic) {
  return (String(MQTT_REALM) + "/devices/" + MQTT_DEVICENAME + "/" + topic);
}

char WillMessage[255];


void setup() {
  Serial.begin(115200);

  // specific Mehdi
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  GetTopic("status/connected").toCharArray(WillMessage, 255);
  client.enableLastWillMessage(WillMessage, "0", true);
  //Serial.println("coucou1");

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  //Serial.println("coucou2");

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  SetAllPixelsColor(255, 0, 0);
  //Serial.println("coucou");
}

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

void DoAnimation(int which) {
  if (which == 1)
    DoAnimationCorruption();
}


void DoAnimationCorruption() {
  unsigned long t0 = millis();
  int t;
  for (int i = 0; i < NUMPIXELS; i+=3) {
    //pixels.clear();
    t = t0 + 200*i;
    // int r = 150.0f + 50.0f*sin((float) t * 0.003f) + 50.0f*sin((float) t * 0.007f);
    // int g = 10.0f + 5.0f*sin((float) t * 0.004f) + 5.0f*sin((float) t * 0.009f);
    int r = 150.0f + 50.0f*sin((float) (i*0.04f + t * 0.001f)*3) + 50.0f*sin((float)  (i*0.03f + t * 0.002f)*7);
    int g = 4.0f + 2.0f*sin((float) t * 0.004f) + 2.0f*sin((float) t * 0.009f);
    pixels.setPixelColor(i, pixels.Color(r, g, 0));
  }
  pixels.show();
}

void OnMessagefromGenZ(const String& payload) {
  client.publish(GetTopic("Wesh").c_str(), payload);
}


// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished() {
  //Serial.println("coucou connected");

  // Subscribe to "mytopic/test" and display received message to Serial
  //client.subscribe(GetTopic("leds/#").c_str(), [](const String & topic, const String & payload) {
  //  client.publish(GetTopic("answer").c_str(),"Le bug est la");
  //});
  client.subscribe(GetTopic("animation/set").c_str(), OnMessageSet);
  client.subscribe(GetTopic("leds/genZ").c_str(), OnMessagefromGenZ);


  ///////
  client.publish(GetTopic("status/connected").c_str(), "1", true);  //true is for retain (durée de vie des messages MQTT)
  pixels.clear();                                                   // Set all pixel colors to 'off'
}

int count = 0;

void SetAllPixelsColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    //pixels.clear();
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}


void loop() {
  client.loop();

  DoAnimation(whichAnimation);
  // count++;
  // count = count%256;
  // if(count<128)
  //   SetAllPixelsColor(2,0,0);
  // else
  //   SetAllPixelsColor(0,0,2);

  //delay(DELAYVAL);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

//ESP Board MAC Address:  EC:FA:BC:2F:ED:FC
//ESP Board MAC Address:  EC:FA:BC:2F:EF:64
