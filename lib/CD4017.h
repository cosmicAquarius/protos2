/**
 * @file CD4017.h
 * @brief Minimal driver class for CD4017 decade counter.
 *
 * Handles clocking and reset operations on the CD4017 IC.
 * Designed for use in microcontroller environments with direct GPIO control.
 */

#ifndef CD4017_H
#define CD4017_H

#include <Arduino.h>

class CD4017
{
public:
    CD4017(uint8_t clkPin, uint8_t rstPin)
        : _clk(clkPin), _rst(rstPin) {}

    void init()
    {
        pinMode(_clk, OUTPUT);
        pinMode(_rst, OUTPUT);
        digitalWrite(_clk, LOW);
        digitalWrite(_rst, LOW);
    }

    /**
     * @brief Sends a rising clock edge to the CD4017 to advance to the next output.
     * Minimum HIGH pulse width is ~100 ns, 1µs is more than enough.
     */
    void clock()
    {
        digitalWrite(_clk, LOW);
        delayMicroseconds(5);
        digitalWrite(_clk, HIGH);
        delayMicroseconds(5);
        digitalWrite(_clk, LOW);
    }

    void clockLow()
    {
        digitalWrite(_clk, LOW);
    }

    void clockHigh()
    {
        digitalWrite(_clk, HIGH);
    }

    /**
     * @brief Resets the CD4017, returning its output to Q0.
     * Reset pulse must be at least 200 ns; 2µs is safe.
     */
    void reset()
    {
        digitalWrite(_rst, LOW);
        delayMicroseconds(5);
        digitalWrite(_rst, HIGH);
        delayMicroseconds(5);
        digitalWrite(_rst, LOW);
    }

    void resetHigh()
    {
     
        digitalWrite(_rst, HIGH);
       
    }

    void resetLow()
    {
     
        digitalWrite(_rst, LOW);
       
    }

private:
    uint8_t _clk;
    uint8_t _rst;
};

#endif // CD4017_H
