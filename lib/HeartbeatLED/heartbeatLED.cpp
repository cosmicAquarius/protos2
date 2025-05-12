#include "HeartbeatLED.h"

void HeartbeatLED::update() {
    unsigned long now = millis();
    if (_state && now - _lastToggle >= _onTime) {
        turnOff();
        _lastToggle = now;
    } else if (!_state && now - _lastToggle >= _offTime) {
        turnOn();
        _lastToggle = now;
    }
}

void HeartbeatLED::turnOn() {
    digitalWrite(_pin, _inverted ? LOW : HIGH);
    _state = true;
}

void HeartbeatLED::turnOff() {
    digitalWrite(_pin, _inverted ? HIGH : LOW);
    _state = false;
}

void HeartbeatLED::setFrequency(unsigned long onDuration, unsigned long offDuration) {
    _onTime = onDuration;
    _offTime = offDuration;
}
