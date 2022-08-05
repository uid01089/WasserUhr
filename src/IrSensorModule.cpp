#include "IrSensorModule.h"
#include "Arduino.h"

IrSensorModule::IrSensorModule()
{
    //
}

IrSensorModule::~IrSensorModule()
{
}

void IrSensorModule::setup(char swithOnDigOutput, char analogInput)
{
    this->analogInput = analogInput;
    this->swithOnDigOutput = swithOnDigOutput;

    pinMode(swithOnDigOutput, OUTPUT);
}

unsigned int IrSensorModule::measure()
{
    // Serial.print("measure: " + String(trigger) + "trigger\n");
    // Serial.print("measure: " + String(echo) + "echo\n");

    int analogValuePre = analogRead(analogInput);
    digitalWrite(swithOnDigOutput, HIGH);
    delay(20);
    int analogValuePost = analogRead(analogInput);
    delay(1);
    digitalWrite(swithOnDigOutput, LOW);

    int analogValue = analogValuePost - analogValuePre;

    // Serial.print("measure: " + String(analogValue) + "\n");

    return analogValue;
}