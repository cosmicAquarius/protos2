#include "Arduino.h"
#include <State.h>
#ifndef Servo_h
#define Servo_h
class Servo
{

public:
    State state = startHigh;
    // the 4017 needs a rizing clock _/- to start a new count
    // so it also need a lower front clock -\_ just after and we fix it a
    // 0.1ms = 100us

    Servo(uint8_t id);
    State nextState();
    void setMin(uint32_t min);
    void setMax(uint32_t max);
    void setPosition(double percent);
    uint32_t getPosition();
    State getState();
    uint8_t getId();
    void reset();

private:
    uint32_t min =  50000;  // 0.5ms
    uint32_t max = 250000; // 2.5ms
    uint8_t _id = 0;
    uint16_t _periodCount = 0;
    uint32_t _targetPosition = 10000; // 1ms safe for init
};
#endif
