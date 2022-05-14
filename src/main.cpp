#include <Arduino.h>
#include <Wire.h>
#include <BfButton.h>
#include <BfButtonManager.h>
#include <RotaryEncoder.h>
#include "ControlSignal.h"
#include "AnalogButtonArray.h"

// Encoders
RotaryEncoder encoderA(ACL, ADA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderB(BCL, BDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderC(CCL, CDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderD(DCL, DDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoders[] = {encoderA, encoderB, encoderC, encoderD};


//encoder buttons
BfButton aSw(BfButton::STANDALONE_DIGITAL, ASW, true, LOW);
BfButton bSw(BfButton::STANDALONE_DIGITAL, BSW, true, LOW);
BfButton cSw(BfButton::STANDALONE_DIGITAL, CSW, true, LOW);
BfButton dSw(BfButton::STANDALONE_DIGITAL, DSW, true, LOW);

AnalogButtonArray analogButtons(A7, 7);

ArrayButton trk1Button(4, 123);
ArrayButton trk2Button(5, 226);
ArrayButton trk3Button(6, 325);
ArrayButton trk4Button(7, 434);

ArrayButton leftButton(8, 568);
ArrayButton rightButton(9, 750);
ArrayButton playButton(10, 1023);


//just easier to loop through all buttons
BfButton* encoderButtons[] = {&aSw, &bSw, &cSw, &dSw};

const uint16_t voltageLimits[][2] = {};
const uint16_t voltageMeans[] = {123, 226, 325, 434, 568, 750, 1023};
//===========BUTTON HANDLING=====================================

uint8_t buttonIndex(BfButton* button)
{
  uint8_t idx = 0;
  while(idx < 4)
  {
    if (encoderButtons[idx] == button)
      return idx;
    ++idx;
  }
  return 0;
}

// send the control signal over I2C
void sendControlSignal(ControlSignal sig)
{
  Wire.beginTransmission(8);
  auto b = sig.asByte();
  ControlSignal::printByte(b);
  Wire.write((uint8_t)b);
  Wire.endTransmission();
}

void buttonPressed (BfButton* btn, BfButton::press_pattern_t pattern)
{
  auto idx = buttonIndex(btn);
  Serial.print("Button ");
  Serial.print(idx);
  Serial.print(" pressed\n");
  ControlSignal sig = {false, false, idx};
  sendControlSignal(sig);
}

int buttonForVoltage(uint16_t v)
{
  if (v < 100)
    return -1;
  for(uint8_t i = 0; i < 7; ++i)
  {
    auto vMax = voltageMeans[i] + 50;
    auto vMin = vMax - 100;
    if (vMax > v && vMin <= v)
    {
      return i + 4;
    }
  }
  return -1;
}


#define DEBOUNCE_MS 200

long lastPressed[7] = {0};

int lastButton = -1;

void checkArrayButtons()
{
  auto now = millis();
  auto voltage = analogRead(A7);
  auto button = buttonForVoltage(voltage);
  if (button == -1)
    return;
  //if lastButton is -1,
  if (lastButton == -1)
  {
    //start the debounce period
    lastPressed[button] = now;
    lastButton = button;
  }
  else if (button == lastButton && now - lastPressed[button] >= DEBOUNCE_MS)
  {
    //debounce period is over, send the button signal and reset
    sendControlSignal({false, false, (uint8_t)button});
    lastButton = -1;
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  for (uint8_t b = 0; b < 7; ++b)
  {
    //arrayButtons[b]->onPress(buttonPressed).onDoublePress(buttonPressed).onPressFor(buttonPressed, 1000);
    //manager.addButton(arrayButtons[b], voltageLimits[b][0], voltageLimits[b][1]);
  }
  // manager.begin();
  Serial.println("Button manager started");
  for (uint8_t i = 0; i < 4; ++i)
  {
    encoderButtons[i]->onPress(buttonPressed).onDoublePress(buttonPressed).onPressFor(buttonPressed, 1000);
  }
  Serial.println("Button callabcks attached");
  Wire.begin(8);
  Serial.println("I2C connection established");
} // setup()

//state of the encoders
int posA = 0;
int posB = 0;
int posC = 0;
int posD = 0;

int* positions[] = {&posA, &posB, &posC, &posD};

// Read the current position of the encoder and print out when changed.
void loop()
{
  //manager.loop();
  checkArrayButtons();
  for(uint8_t b = 0; b < 4; ++b)
  {
    encoderButtons[b]->read();
  }
  for(uint8_t i = 0; i < 4; ++i)
  {
    encoders[i].tick();
    int newPos = encoders[i].getPosition();
    if (newPos != *positions[i])
    {
      Serial.print("Encoder ");
      Serial.print(i);
      Serial.print(" position: ");
      Serial.print(newPos);
      Serial.print(" with direction ");
      bool dir = newPos >= *positions[i];
      Serial.println(dir);
      ControlSignal sig = {true, dir, i};
      sendControlSignal(sig);
      Serial.println("I2C message sent");
      *positions[i] = newPos;
    }
  }
 
} // loop ()

// The End