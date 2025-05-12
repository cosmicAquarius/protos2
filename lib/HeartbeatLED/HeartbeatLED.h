#ifndef HEARTBEAT_LED_H
#define HEARTBEAT_LED_H

#include <Arduino.h>

class HeartbeatLED {
public:

    HeartbeatLED(uint8_t pin, unsigned long onDuration = 100, unsigned long offDuration = 900, bool invertedLogic = true)
        : _pin(pin), _onTime(onDuration), _offTime(offDuration), _inverted(invertedLogic) {
        pinMode(_pin, OUTPUT);
        turnOff();
    }

    void update();
    void turnOn();
    void turnOff();
    void setFrequency(unsigned long onDuration, unsigned long offDuration);

private:
    uint8_t _pin;
    unsigned long _onTime;
    unsigned long _offTime;
    unsigned long _lastToggle = 0;
    bool _inverted;
    bool _state = false;
};

#endif
