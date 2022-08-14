#include "WaterMeasurement.h"
#include <functional>

#define MQTT_TRANSMIT_CTL_ANALOG "/control/rawvalue"
#define MQTT_TRANSMIT_CTL_LOGIC "/control/logicvalue"
#define MQTT_TRANSMIT_CTL_ADAPT "/control/adapt"
#define IR_INPUT A0
#define SWITCH_ON D2
#define NR_CYCLES_BEFORE_ADAPTATION 5
#define TRIGGER_LOW_LIMIT (unsigned int)600
#define TRIGGER_HIGH_LIMIT (unsigned int)700
#define TRIGGER_LOW_LIMIT_PER (unsigned int)20
#define TRIGGER_HIGH_LIMIT_PER (unsigned int)20

Watermeasurement::Watermeasurement(KStandardCore *kStandardCore)
{
    this->kStandardCore = kStandardCore;
}

Watermeasurement::~Watermeasurement()
{
}

void Watermeasurement::setup()
{
    publishRawValues = false;
    publishLogicValues = false;
    waitCycleCtrForAdaptation = NR_CYCLES_BEFORE_ADAPTATION;

    irSensorModule.setup(SWITCH_ON, IR_INPUT);

    trigger.setup(TRIGGER_LOW_LIMIT, TRIGGER_HIGH_LIMIT, TRIGGER_LOW_LIMIT_PER, TRIGGER_HIGH_LIMIT_PER);

    kStandardCore->getKMqtt()->regCallBack("/" + kStandardCore->getHostname() + MQTT_TRANSMIT_CTL_ANALOG, std::bind(&Watermeasurement::mqttAnalog, this, std::placeholders::_1));
    kStandardCore->getKMqtt()->regCallBack("/" + kStandardCore->getHostname() + MQTT_TRANSMIT_CTL_LOGIC, std::bind(&Watermeasurement::mqttDigital, this, std::placeholders::_1));
    kStandardCore->getKMqtt()->regCallBack("/" + kStandardCore->getHostname() + MQTT_TRANSMIT_CTL_ADAPT, std::bind(&Watermeasurement::mqttAdaptation, this, std::placeholders::_1));

    clcWaterMeasurement_500ms();
}

void Watermeasurement::loop()
{
}

void Watermeasurement::clcWaterMeasurement_500ms(void)
{
    unsigned int distance = irSensorModule.measure();
    const bool logicalValue = trigger.get(distance);

    if (publishRawValues)
    {
        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/rawvalue", String(distance).c_str());
    }

    if (publishLogicValues)
    {
        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/logic", String(logicalValue).c_str());
    }

    // Check if complete cycle
    if (fallingEdge.get(logicalValue))
    {
        // We have one complete cylce, do the needful things

        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/one_litre_tick", "1");

        kStandardCore->getNTPClient()->update();
        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/time", kStandardCore->getNTPClient()->getTimeString().c_str());

        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/highlimit", String(trigger.getHighLimit()).c_str());
        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/lowlimit", String(trigger.getLowLimit()).c_str());
        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/maxvalue", String(trigger.getMaxValue()).c_str());
        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/minvalue", String(trigger.getMinValue()).c_str());
        kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/waitCycleCtrForAdaptation", String(waitCycleCtrForAdaptation).c_str());

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
    kStandardCore->getKSchedule()->schedule(std::bind(&Watermeasurement::clcWaterMeasurement_500ms, this), 500);
}

void Watermeasurement::mqttAnalog(String value)
{
    Serial.print("mqttAnalog\n");
    Serial.print(value);

    if (value == "true")
    {
        publishRawValues = true;
    }
    else if (value == "false")
    {
        publishRawValues = false;
    }
}
void Watermeasurement::mqttDigital(String value)
{
    if (value == "true")
    {
        publishLogicValues = true;
    }
    else if (value == "false")
    {
        publishLogicValues = false;
    }
}

void Watermeasurement::mqttAdaptation(String value)
{

    kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/highlimit", String(trigger.getHighLimit()).c_str());
    kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/lowlimit", String(trigger.getLowLimit()).c_str());
    kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/maxvalue", String(trigger.getMaxValue()).c_str());
    kStandardCore->getKMqtt()->publish("/" + kStandardCore->getHostname() + "/minvalue", String(trigger.getMinValue()).c_str());

    if (value == "true")
    {
        waitCycleCtrForAdaptation = NR_CYCLES_BEFORE_ADAPTATION;
        trigger.setup(TRIGGER_LOW_LIMIT, TRIGGER_HIGH_LIMIT, TRIGGER_LOW_LIMIT_PER, TRIGGER_HIGH_LIMIT_PER);
    }
    else if (value == "false")
    {
        waitCycleCtrForAdaptation = -1;
        trigger.setup(TRIGGER_LOW_LIMIT, TRIGGER_HIGH_LIMIT, TRIGGER_LOW_LIMIT_PER, TRIGGER_HIGH_LIMIT_PER);
    }
}