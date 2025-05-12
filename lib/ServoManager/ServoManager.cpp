#include "ServoManager.h"

ServoManager::ServoManager()
{
    for (uint8_t i = 0; i < 8; i++)
    {
        _servos[i] = new Servo(i);
    }
    _currentServo = 0;
}

bool ServoManager::isFirst() {
    return _currentServo == 0;
}
bool ServoManager::isAllDone()
{
    if (_servos[_currentServo]->getState() == finishLow)
    {
        if (_currentServo >= 8)
        {
            return true;
        }
    }
    return false;
}

void ServoManager::nextServo() {
    _currentServo++;
    //servo 9 does not exists 
    //when servo 8 is triggered low
    //the 4017 will put output 9 high until reset
    if (_currentServo >= 9)
    {
        _currentServo = 0;
        
    }
}

Servo *ServoManager::getCurrent()
{

    return _servos[_currentServo];
}

Servo *ServoManager::getServo(uint8_t servoIndex)
{

    return _servos[servoIndex];
}
void ServoManager::resetAll() {
    for (uint8_t i = 0; i < 8; i++) {
        _servos[i]->reset();
    }
    _currentServo = 0;
}