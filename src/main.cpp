#include <Arduino.h>
#include <KStandardCore.h>
#include <WaterMeasurement.h>
#include <Waterdetection.h>
#include "incredential.h"

#include <KBlink.h>

// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

KStandardCore kStandardCore;
KBlink kblink;

Watermeasurement watermeasurement(&kStandardCore);
Waterdetection waterdetection(&kStandardCore);

void setup()
{
  Serial.begin(9600); // Baudrate: 9600

  kStandardCore.setup("Wasseruhr", SSID, PSK, MQTT_BROKER, 1883);

  watermeasurement.setup();
  Serial.print("Watermeasurement start done\n");

  waterdetection.setup();
  Serial.print("Waterdetection start done\n");
  
  kblink.setup(kStandardCore.getKSchedule(), 100, 3000);

}

void loop()
{
  kStandardCore.loop();
  kblink.loop();
  watermeasurement.loop();
  waterdetection.loop();
}
