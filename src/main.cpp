#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <KSchedule.h>
#include <KMqtt.h>
#include <NTPClient.h>
#include <WaterMeasurement.h>
#include <Waterdetection.h>

#include <main.h>
#include "incredential.h"

// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

const long utcOffsetInSeconds = 3600;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
KSchedule kSchedule;
KMqtt kmqtt(mqttClient);

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

Watermeasurement watermeasurement(kmqtt, kSchedule, timeClient);
Waterdetection waterdetection(kmqtt, kSchedule);

// Function Prototypes
void setupWifi();

/**
 * Performing one time setup after boot.
 *
 */
void setup()
{
  Serial.begin(9600); // Baudrate: 9600

  setupWifi();
  Serial.print("Wifi Setup done\n");

  kmqtt.setup(MQTT_BROKER, 1883, "Wasseruhr");
  Serial.print("MQTT Setup done\n");

  timeClient.begin();
  Serial.print("TimeClient Setup done\n");

  kSchedule.setup();
  Serial.print("KSchedule start done\n");

  watermeasurement.setup();
  Serial.print("Watermeasurement start done\n");

  waterdetection.setup();
  Serial.print("Waterdetection start done\n");

  kmqtt.publish("/heizung/wasseruhr/reset", "1");
}

void setupWifi()
{
  WiFi.hostname("Wasseruhr");
  WiFi.begin(SSID, PSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  // wifi_set_sleep_type(LIGHT_SLEEP_T);
}

void loop()
{

  kSchedule.loop();
  mqttClient.loop();
  watermeasurement.loop();
  waterdetection.loop();
  delay(100);
}
