#ifndef TRIGGER_H
#define TRIGGER_H

class Trigger
{

private:
    bool value;
    unsigned int highLimit;
    unsigned int lowLimit;
    unsigned int lowLimitPercent;
    unsigned int highLimitPercent;
    unsigned int maxValue;
    unsigned int minValue;

public:
    Trigger();
    ~Trigger();

    void setup(unsigned lowLimit, unsigned highLimit, unsigned lowLimitPercent, unsigned highLimitPercent);

    bool get(unsigned rawValue);
    unsigned int getMaxValue();
    unsigned int getMinValue();
    unsigned int getHighLimit();
    unsigned int getLowLimit();
    void adapt();
};

#endif