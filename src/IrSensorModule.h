#ifndef IRSENSORMODULE_H
#define IRSENSORMODULE_H

class IrSensorModule
{

private:
    unsigned char analogInput;
    unsigned char swithOnDigOutput;

public:
    IrSensorModule();
    ~IrSensorModule();

    void setup(char swithOnDigOutput, char analogInput);

    unsigned int measure();
};

#endif