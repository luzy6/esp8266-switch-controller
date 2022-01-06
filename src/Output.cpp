#include <Output.h>

Output::Output(int pin): m_pin(pin) {
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);
}

void Output::on(){
    digitalWrite(m_pin, HIGH);
}

void Output::off(){
    digitalWrite(m_pin, LOW);
}

void Output::reverse(){
    digitalWrite(m_pin, !digitalRead(m_pin));
}