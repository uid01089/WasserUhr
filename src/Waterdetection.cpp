#include <Waterdetection.h>

#define WATER_DETECTION_INPUT D5
#define WATER_DETECTION_GND D1

Waterdetection::Waterdetection(KStandardCore *kStandardCore)
{
    this->kStandardCore = kStandardCore;
}
Waterdetection::~Waterdetection() {}

void Waterdetection::setup()
{
    pinMode(WATER_DETECTION_GND, OUTPUT);
    digitalWrite(WATER_DETECTION_GND, LOW);

    pinMode(WATER_DETECTION_INPUT, INPUT_PULLUP);

    clcWaterDetection_5s();
}
void Waterdetection::loop() {}

void Waterdetection::clcWaterDetection_5s()
{
    int waterDetected = digitalRead(WATER_DETECTION_INPUT);
    kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/waterdetection", String(waterDetected).c_str());

    // Reschedule it again
    kStandardCore->getKSchedule()->schedule(std::bind(&Waterdetection::clcWaterDetection_5s, this), 5000);
}
