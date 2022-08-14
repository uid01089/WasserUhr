#ifndef WATERDETECTION_H
#define WATERDETECTION_H

#include <KStandardCore.h>
#include <Arduino.h>

class Waterdetection
{

private:
    void clcWaterDetection_5s();
    KStandardCore* kStandardCore;



public:
    Waterdetection(KStandardCore* kStandardCore);
    ~Waterdetection();

    void setup();
    void loop();
};



#endif