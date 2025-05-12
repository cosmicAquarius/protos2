#include "Servo.h"

Servo::Servo(uint8_t id)
{
    _id = id;
}
State Servo::getState()
{

    return state;
}
State Servo::nextState()
{

    switch (state)
    {
    case startHigh:
        state = variablePhase;
        break;
    case variablePhase:
        state = finishLow;
        break;
    case finishLow:
        state = startHigh;
        break;
    }
    return state;
}
void Servo::setPosition(double percent)
{
    if (percent < 0)
    {
        percent = 0;
    }
    if (percent > 180)
    {
        percent = 180;
    }

    _targetPosition = min + round((double)((max - min) / 100) * percent);
}
uint32_t Servo::getPosition()
{
    return _targetPosition;
}
uint8_t Servo::getId()
{
    return _id;
}
void Servo::setMin(uint32_t min)
{
    this->min = min;
}
void Servo::setMax(uint32_t max)
{
    this->max = max;
}
void Servo::reset()
{
    state = startHigh;
    _periodCount = 0;
    _targetPosition = min + (max - min) / 2; // Position neutre 90°
}
