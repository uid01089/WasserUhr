#ifndef WATERMEASUREMENT_H
#define WATERMEASUREMENT_H

#include <KStandardCore.h>
#include <Trigger.h>
#include <FallingEdge.h>
#include <IrSensorModule.h>
#include <Arduino.h>

class Watermeasurement
{

private:
    Trigger trigger;
    FallingEdge fallingEdge;
    bool publishRawValues;
    bool publishLogicValues;
    signed int waitCycleCtrForAdaptation;
    IrSensorModule irSensorModule;
    KStandardCore* kStandardCore;
    void clcWaterMeasurement_500ms(void);
    void mqttAnalog(String value);
    void mqttDigital(String value);
    void mqttAdaptation(String value);

public:
    Watermeasurement(KStandardCore* kStandardCore);
    ~Watermeasurement();

    void setup();
    void loop();
};

#endif