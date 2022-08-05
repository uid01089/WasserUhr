#include "Trigger.h"
#include <algorithm> // std::min

Trigger::Trigger()
{
    value = false;
    this->minValue = 0xffff;
    this->maxValue = 0;
}

Trigger::~Trigger()
{
}

void Trigger::setup(unsigned lowLimit, unsigned highLimit, unsigned lowLimitPercent, unsigned highLimitPercent)
{
    this->lowLimit = lowLimit;
    this->highLimit = highLimit;
    this->lowLimitPercent = lowLimitPercent;
    this->highLimitPercent = highLimitPercent;
}

bool Trigger::get(unsigned rawValue)
{

    minValue = std::min(rawValue, minValue);
    maxValue = std::max(rawValue, maxValue);

    if (value)
    {
        // We have to reach the lowLimit to change the value
        if (rawValue < lowLimit)
        {
            value = false;
        }
    }
    else
    {
        // We have to reach teh highLimit to change the value
        if (rawValue > highLimit)
        {
            value = true;
        }
    }
    return value;
}

unsigned int Trigger::getMaxValue()
{
    return maxValue;
}

unsigned int Trigger::getMinValue()
{
    return minValue;
}

unsigned int Trigger::getHighLimit()
{
    return highLimit;
}

unsigned int Trigger::getLowLimit()
{
    return lowLimit;
}

void Trigger::adapt()
{

    unsigned long delta = maxValue - minValue;

    highLimit = maxValue - ((unsigned long)delta) * highLimitPercent / 100;
    lowLimit = minValue + ((unsigned long)delta) * lowLimitPercent / 100;

    maxValue = 0;
    minValue = 0xffff;
}
