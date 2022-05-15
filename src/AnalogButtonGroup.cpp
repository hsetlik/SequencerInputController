#include "AnalogButtonGroup.h"

AnalogButton::AnalogButton(uint8_t id, uint16_t v) : 
index(id), 
voltage(v),
isPressed(false),
pressedAt(0),
holdLength(300),
fPress(0),
fHold(0)
{

}
    
AnalogButtonGroup::AnalogButtonGroup(uint8_t p, uint8_t n) :
pin(p),
numButtons(n),
lastButton(0)
{
    buttons = new AnalogButton[numButtons];
    now = millis();
}

void AnalogButtonGroup::update()
{
    if (millis() - now >= MIN_INTERVAL)
    {
        now = millis();
        auto reading = analogRead(pin);
    }
}
    
AnalogButton* AnalogButtonGroup::buttonForVoltage(uint16_t v)
{
    for (uint8_t i = 0; i < numButtons; i++)
    {
        auto button = &buttons[i];
        if (button->voltage + MV_MARGIN > v && button->voltage - MV_MARGIN <= v)
            return button;
    }
    return nullptr;
}