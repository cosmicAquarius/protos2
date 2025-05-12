#include "Arduino.h"
#include <Servo.h>
#ifndef ServoManager_h
#define ServoManager_h
class ServoManager
{

public:
    ServoManager();
    bool isAllDone();
    bool isFirst();

    Servo *getCurrent();
    Servo *getServo(uint8_t servoIndex);
    void nextServo();
    void resetAll();

private:
     Servo *_servos[8];
     uint8 _currentServo = 0;
};
#endif