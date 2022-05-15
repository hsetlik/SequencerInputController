#ifndef ANALOGBUTTONGROUP_H
#define ANALOGBUTTONGROUP_H
#include <Arduino.h>

#define MAX_BUTTONS 10
#define MIN_INTERVAL 20
#define MV_MARGIN 50

typedef void (*ButtonEvent)(uint8_t);

class AnalogButton
{
    friend class AnalogButtonGroup;
public:
    AnalogButton(uint8_t id=0, uint16_t voltage=0);
    const uint8_t index;
    const uint8_t voltage;
    AnalogButton& onPress(ButtonEvent func)
    {
        fPress = func;
        return *this;
    }
    AnalogButton& onHold(ButtonEvent event, uint16_t length)
    {
        holdLength = length;
        fHold = event;
        return *this;
    }
private:
    bool isPressed;
    unsigned long pressedAt;
    uint16_t holdLength;
    ButtonEvent fPress;
    ButtonEvent fHold;
};
//============================================================
class AnalogButtonGroup 
{
public:
    AnalogButtonGroup(uint8_t pin=A0, uint8_t numButtons=MAX_BUTTONS);
    //Call this in the `loop()` function to trigger correct button events
    const uint8_t pin;
    const uint8_t numButtons;
    void update();
    AnalogButton* buttonForVoltage(uint16_t v);
private:
    unsigned long now;
    AnalogButton* buttons;
    uint8_t lastButton;


};

#endif