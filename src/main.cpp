#include <Arduino.h>
#include <Wire.h>
#include <BfButton.h>
#include <BfButtonManager.h>
#include <RotaryEncoder.h>
#include "ControlSignal.h"
//just easier to loop through all buttons

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

//BfButtonManager manager(A7, 7);
//left/right menu buttons
//Track select buttons
BfButton trk1Button(BfButton::ANALOG_BUTTON_ARRAY, 4);
BfButton trk2Button(BfButton::ANALOG_BUTTON_ARRAY, 5);
BfButton trk3Button(BfButton::ANALOG_BUTTON_ARRAY, 6);
BfButton trk4Button(BfButton::ANALOG_BUTTON_ARRAY, 7);

BfButton playButton(BfButton::ANALOG_BUTTON_ARRAY, 8);

BfButton leftSw(BfButton::ANALOG_BUTTON_ARRAY, 9);
BfButton rightSw(BfButton::ANALOG_BUTTON_ARRAY, 10);

//just easier to loop through all buttons
BfButton* encoderButtons[] = {&aSw, &bSw, &cSw, &dSw};
BfButton* arrayButtons[] = {&trk1Button, &trk2Button, &trk3Button, &trk4Button, &playButton, &leftSw, &rightSw};

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
  idx = 0;
  while (idx < 7)
  {
    if (arrayButtons[idx] == button)
      return idx + 4;
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

uint8_t buttonForVoltage(uint16_t v)
{
  for(uint8_t i = 0; i < 7; ++i)
  {
    auto vMax = voltageMeans[i] + 50;
    auto vMin = vMax - 100;
    if (vMax > v && vMin <= v)
    {
      return i + 4;
    }
  }
  return 0;
}

int lastButton = -1;
unsigned long lastPressedAt = 0;
void checkArrayButtons()
{
  auto vButton = analogRead(A7);
  auto idx = buttonForVoltage(vButton);
  auto now = millis();
  if (vButton > 100 && lastButton != idx)
  {
    lastPressedAt = now;
    //Serial.println(vButton);
    sendControlSignal({false, false, idx});
    lastButton = idx;
  }
  if (now - lastPressedAt > 600)
  {
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