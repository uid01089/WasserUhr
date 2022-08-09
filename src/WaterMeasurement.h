#ifndef WATERMEASUREMENT_H
#define WATERMEASUREMENT_H

#include <KSchedule.h>
#include <KMqtt.h>
#include <Trigger.h>
#include <FallingEdge.h>
#include <IrSensorModule.h>
#include <NTPClient.h>
#include <Arduino.h>

class Watermeasurement
{

private:
    KMqtt *kmqtt;
    KSchedule *kschedule;
    NTPClient *timeClient;
    Trigger trigger;
    FallingEdge fallingEdge;
    bool publishRawValues;
    bool publishLogicValues;
    signed int waitCycleCtrForAdaptation;
    IrSensorModule irSensorModule;
    void clcWaterMeasurement_500ms(void);
    void mqttAnalog(String value);
    void mqttDigital(String value);
    void mqttAdaptation(String value);

public:
    Watermeasurement(KMqtt &kmqtt, KSchedule &kschedule, NTPClient& timeClient);
    ~Watermeasurement();

    void setup();
    void loop();
};

#endif