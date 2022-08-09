#include <Waterdetection.h>

#define WATER_DETECTION_INPUT D5
#define WATER_DETECTION_GND D1

Waterdetection::Waterdetection(KMqtt &kmqtt, KSchedule &kschedule)
{
    this->kmqtt = &kmqtt;
    this->kschedule = &kschedule;
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
    kmqtt->publish("/heizung/wasseruhr/waterdetection", String(waterDetected).c_str());

    // Reschedule it again
    kschedule->schedule(std::bind(&Waterdetection::clcWaterDetection_5s, this), 5000);
}
