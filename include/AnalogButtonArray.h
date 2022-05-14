#ifndef ANALOGBUTTONARRAY_H
#define ANALOGBUTTONARRAY_H
#include <Arduino.h>
#define DEBOUNCE_LENGTH 200
struct ArrayButton
{
    uint8_t id;
    uint16_t minVoltage;
    uint16_t maxVoltage;
    ArrayButton() : id(0), minVoltage(0), maxVoltage(1023){}
    ArrayButton(uint8_t idx, uint16_t mean, uint16_t margin=25) :
    id(idx), minVoltage(mean - margin), maxVoltage(mean + margin){}
    ArrayButton(uint8_t idx, uint16_t vMin, uint16_t vMax) : 
    id(idx), minVoltage(vMin), maxVoltage(vMax){}
};

typedef void (*ButtonCallback)(uint8_t);

 void nullCallback(uint8_t){};

class AnalogButtonArray
{
public:
    AnalogButtonArray(uint8_t pin, uint8_t buttonCount);
    void attachButton(ArrayButton button, ButtonCallback func=&nullCallback);
    void tick();
    void setCallback(uint8_t buttonIdx, ButtonCallback callback);
private:
    const uint8_t inputPin;
    const uint8_t numButtons;
    ArrayButton *buttons;
    ButtonCallback *callbacks;
    uint8_t lastButtonId;
    unsigned long lastButtonSetAt;
    ArrayButton *buttonForVoltage(int voltage);
};

#endif