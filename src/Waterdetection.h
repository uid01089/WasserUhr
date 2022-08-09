#ifndef WATERDETECTION_H
#define WATERDETECTION_H

#include <KSchedule.h>
#include <KMqtt.h>
#include <Arduino.h>

class Waterdetection
{

private:
    KMqtt *kmqtt;
    KSchedule *kschedule;
    void clcWaterDetection_5s();


public:
    Waterdetection(KMqtt &kmqtt, KSchedule &kschedule);
    ~Waterdetection();

    void setup();
    void loop();
};



#endif