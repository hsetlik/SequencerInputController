#include "AnalogButtonArray.h"

AnalogButtonArray::AnalogButtonArray(uint8_t pin, uint8_t n) : inputPin(pin), numButtons(n)
{
    buttons = new ArrayButton[numButtons];
    callbacks = new ButtonCallback[numButtons];
}

void AnalogButtonArray::attachButton(ArrayButton button, ButtonCallback func)
{
    buttons[button.id] = button;
    callbacks[button.id] = func;
}

ArrayButton* AnalogButtonArray::buttonForVoltage(int voltage)
{
    if(voltage < buttons[0].minVoltage)
        return nullptr;
    for(uint8_t i = 0; i < numButtons; ++i)
    {
        auto button = &buttons[i];
        if (button->minVoltage <= voltage && voltage < button->maxVoltage)
            return button;
    }
    return nullptr;
}

void AnalogButtonArray::setCallback(uint8_t idx, ButtonCallback func)
{
    callbacks[idx] = func;
}

void AnalogButtonArray::tick()
{
    auto now = millis();
    auto button = buttonForVoltage(analogRead(inputPin));
    if (button == nullptr)
        return;
    if (button->id == lastButtonId && now - lastButtonSetAt >= DEBOUNCE_LENGTH)
    {
        auto func = callbacks[button->id];
        func(button->id);
    }
    else if(button->id != lastButtonId)
    {
        lastButtonSetAt = now;
        lastButtonId = button->id;
    }
}

