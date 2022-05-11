#include <Arduino.h>
#include <Wire.h>
#include <BfButton.h>
#include <BfButtonManager.h>
#include <RotaryEncoder.h>
#include "ControlSignal.h"


//encoder buttons
BfButton aSw(BfButton::STANDALONE_DIGITAL, ASW, true, LOW);
BfButton bSw(BfButton::STANDALONE_DIGITAL, BSW, true, LOW);
BfButton cSw(BfButton::STANDALONE_DIGITAL, CSW, true, LOW);
BfButton dSw(BfButton::STANDALONE_DIGITAL, DSW, true, LOW);

//left/right menu buttons
BfButton leftSw(BfButton::STANDALONE_DIGITAL, LBUTTON, true, LOW);
BfButton rightSw(BfButton::STANDALONE_DIGITAL, RBUTTON, true, LOW);

//Track select buttons
BfButton trk1Button(BfButton::STANDALONE_DIGITAL, TRK1, true, LOW);
BfButton trk2Button(BfButton::STANDALONE_DIGITAL, TRK2, true, LOW);
BfButton trk3Button(BfButton::STANDALONE_DIGITAL, TRK3, true, LOW);
BfButton trk4Button(BfButton::STANDALONE_DIGITAL, TRK4, true, LOW);


BfButton playButton(BfButton::STANDALONE_DIGITAL, PLAY, true, LOW);

//just easier to loop through all buttons
BfButton* const allButtons[] = {&aSw, &bSw, &cSw, &dSw, &leftSw, &rightSw, &trk1Button, &trk2Button, &trk3Button, &trk4Button, &playButton};

// Encoders
RotaryEncoder encoderA(ACL, ADA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderB(BCL, BDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderC(CCL, CDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderD(DCL, DDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoders[] = {encoderA, encoderB, encoderC, encoderD};

//===========BUTTON HANDLING=====================================

uint8_t buttonIndex(BfButton* button)
{
  uint8_t idx = 0;
  while(idx > 11)
  {
    if (allButtons[idx] == button)
      return idx;
  }
  return 0;
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

// send the control signal over I2C
void sendControlSignal(ControlSignal sig)
{
  Wire.beginTransmission(8);
  Wire.write((uint8_t)sig.asByte());
  Wire.endTransmission();
}


void setup()
{
  Serial.begin(9600);
  while (! Serial);
  
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
  for(uint8_t b = 0; b < 11; ++b)
  {
    allButtons[b]->read();
  }
} // loop ()

// The End