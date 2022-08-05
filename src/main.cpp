#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <KSchedule.h>
#include <Trigger.h>
#include <NTPClient.h>
#include <FallingEdge.h>

#include <main.h>
#include "incredential.h"

#define IR_INPUT A0
#define SWITCH_ON D2
#define WATER_DETECTION_INPUT D5
#define WATER_DETECTION_GND D1
#define NR_CYCLES_BEFORE_ADAPTATION 5

// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

const char *MQTT_TRANSMIT_ANALOG = "/heizung/wasseruhr/control/rawvalue";
const char *MQTT_TRANSMIT_LOGIC = "/heizung/wasseruhr/control/logicvalue";

const long utcOffsetInSeconds = 3600;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
IrSensorModule irSensorModule;
WiFiUDP ntpUDP;
KSchedule kSchedule;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);
Trigger trigger;
FallingEdge fallingEdge;

bool publishRawValues = false;
bool publishLogicValues = false;
int waitCycleCtrForAdaptation = NR_CYCLES_BEFORE_ADAPTATION;

// Function Prototypes
void setupWifi();
void setupMqtt();
void clcWaterMeasurement_500ms();
void clcWaterDetection_5s();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void setupWaterDetection();
void setupWaterMeasurement();

/**
 * Performing one time setup after boot.
 *
 */
void setup()
{
  Serial.begin(9600); // Baudrate: 9600

  setupWifi();
  Serial.print("Wifi Setup done\n");

  setupMqtt();
  Serial.print("MQTT Setup done\n");

  timeClient.begin();
  Serial.print("TimeClient Setup done\n");

  kSchedule.setup();
  Serial.print("KSchedule start done\n");

  setupWaterMeasurement();
  setupWaterDetection();
}

void setupWaterMeasurement()
{
  publishRawValues = false;
  publishLogicValues = false;
  waitCycleCtrForAdaptation = NR_CYCLES_BEFORE_ADAPTATION;

  irSensorModule.setup(SWITCH_ON, IR_INPUT);
  Serial.print("Ultrasonic Setup done\n");

  trigger.setup(600, 700, 20, 20);
  Serial.print("Trigger Setup done\n");

  clcWaterMeasurement_500ms();
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

void setupMqtt()
{
  mqttClient.setServer(MQTT_BROKER, 1883);
  while (!mqttClient.connected())
  {
    mqttClient.connect("Wasseruhr");
    delay(100);
  }
  mqttClient.subscribe(MQTT_TRANSMIT_ANALOG);
  mqttClient.subscribe(MQTT_TRANSMIT_LOGIC);
  mqttClient.setCallback(mqttCallback);
}

void setupWaterDetection()
{
  pinMode(WATER_DETECTION_GND, OUTPUT);
  digitalWrite(WATER_DETECTION_GND, LOW);

  pinMode(WATER_DETECTION_INPUT, INPUT_PULLUP);

  clcWaterDetection_5s();
}

void loop()
{

  kSchedule.loop();
  mqttClient.loop();
  delay(100);
}

void clcWaterMeasurement_500ms()
{
  unsigned int distance = irSensorModule.measure();
  const bool logicalValue = trigger.get(distance);

  if (publishRawValues)
  {
    mqttClient.publish("/heizung/wasseruhr/rawvalue", String(distance).c_str());
  }

  if (publishLogicValues)
  {
    mqttClient.publish("/heizung/wasseruhr/logic", String(logicalValue).c_str());
  }

  // Check if complete cycle
  if (fallingEdge.get(logicalValue))
  {
    // We have one complete cylce, do the needful things

    mqttClient.publish("/heizung/wasseruhr/one_litre_tick", "1");

    timeClient.update();
    mqttClient.publish("/heizung/wasseruhr/time", timeClient.getFormattedTime().c_str());
    // Serial.print("time: " + timeClient.getFormattedTime() + "\n");

    mqttClient.publish("/heizung/wasseruhr/rssi", String(WiFi.RSSI()).c_str());

    mqttClient.publish("/heizung/wasseruhr/highlimit", String(trigger.getHighLimit()).c_str());
    mqttClient.publish("/heizung/wasseruhr/lowlimit", String(trigger.getLowLimit()).c_str());
    mqttClient.publish("/heizung/wasseruhr/maxvalue", String(trigger.getMaxValue()).c_str());
    mqttClient.publish("/heizung/wasseruhr/minvalue", String(trigger.getMinValue()).c_str());
    mqttClient.publish("/heizung/wasseruhr/waitCycleCtrForAdaptation", String(waitCycleCtrForAdaptation).c_str());

    if (waitCycleCtrForAdaptation == 0)
    {
      trigger.adapt();
    }
    else
    {
      waitCycleCtrForAdaptation--;
    }
  }

  // Reschedule it again
  kSchedule.schedule(clcWaterMeasurement_500ms, 500);
}

void clcWaterDetection_5s()
{
  int waterDetected = digitalRead(WATER_DETECTION_INPUT);
  mqttClient.publish("/heizung/wasseruhr/waterdetection", String(waterDetected).c_str());

  // Reschedule it again
  kSchedule.schedule(clcWaterDetection_5s, 5000);
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  char messageBuffer[length];             // somewhere to put the message
  memcpy(messageBuffer, payload, length); // copy in the payload
  messageBuffer[length] = '\0';

  if (strcmp(topic, MQTT_TRANSMIT_ANALOG) == 0)
  {
    if (strcmp(messageBuffer, "true") == 0)
    {
      publishRawValues = true;
    }
    else if (strcmp(messageBuffer, "false") == 0)
    {
      publishRawValues = false;
    }
  }
  else if (strcmp(topic, MQTT_TRANSMIT_LOGIC) == 0)
  {
    if (strcmp(messageBuffer, "true") == 0)
    {
      publishLogicValues = true;
    }
    else if (strcmp(messageBuffer, "false") == 0)
    {
      publishLogicValues = false;
    }
  }

  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");
}