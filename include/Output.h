#ifndef OUTPUR_H
#define OUTPUR_H

#include <Arduino.h>

class Output {
    public:
        Output(int pin);
        void on();
        void off();
        void reverse();
    private:
        uint8_t m_pin;
};

#endif