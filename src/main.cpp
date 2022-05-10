#include <Arduino.h>
#include <Wire.h>
#include <BfButton.h>
#include <BfButtonManager.h>
#include <RotaryEncoder.h>

// Pin definifitons
//Written for Arduino Nano as per KiCad schematic
#define ACL 0
#define ADA 1
#define ASW 2

#define BCL 3
#define BDA 4
#define BSW 5

#define CCL 6
#define CDA 7
#define CSW 8

#define DCL 9
#define DDA 10
#define DSW 11

#define LBUTTON A0
#define RBUTTON A1

#define PLAY A2

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoderA(A1, A2, RotaryEncoder::LatchMode::FOUR3);

const uint8_t buttonPins[] = {ASW, BSW, CSW, DSW};
BfButton aSw(BfButton::STANDALONE_DIGITAL, ASW, true, LOW);
BfButton bSw(BfButton::STANDALONE_DIGITAL, BSW, true, LOW);
BfButton cSw(BfButton::STANDALONE_DIGITAL, CSW, true, LOW);
BfButton dSw(BfButton::STANDALONE_DIGITAL, DSW, true, LOW);

BfButton* encoderButtons[] = {&aSw, &bSw, &cSw, &dSw};

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoderA(ACL, ADA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderB(BCL, BDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderC(CCL, CDA, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderD(DCL, DDA, RotaryEncoder::LatchMode::FOUR3);


RotaryEncoder encoders[] = {encoderA, encoderB, encoderC, encoderD};

void handleA (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch A pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(0); //Write button index;
  Wire.write(0); //low bc direction is not used
  Wire.endTransmission();
  Serial.println("Switch message sent");
}

void handleB (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch B pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(1); //Write button index;
  Wire.write(0);
  Wire.endTransmission();
  Serial.println("Switch message sent");
}

void handleC (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch C pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(2); //Write button index;
  Wire.write(0);
  Wire.endTransmission();
  Serial.println("Switch message sent");
}

void handleD (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch D pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(3); //Write button index;
  Wire.write(0);
  Wire.endTransmission();
  Serial.println("Switch message sent");
}

// handle button presses generically rather than needing a new handler for each button

void setup()
{
  Serial.begin(9600);
  while (! Serial);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");

  aSw.onPress(handleA)
    .onDoublePress(handleA)
    .onPressFor(handleA, 1000);
 
  bSw.onPress(handleB)
    .onDoublePress(handleB)
    .onPressFor(handleB, 1000);
  
  cSw.onPress(handleC)
    .onDoublePress(handleC)
    .onPressFor(handleC, 1000);
 
  dSw.onPress(handleD)
    .onDoublePress(handleD)
    .onPressFor(handleD, 1000);

  Wire.begin();
 
} // setup()
int posA = 0;
int posB = 0;
int posC = 0;
int posD = 0;

int* positions[] = {&posA, &posB, &posC, &posD};

// Read the current position of the encoder and print out when changed.
void loop()
{
  for(int i = 0; i < 4; ++i)
  {
    encoderButtons[i]->read();
    encoders[i].tick();
    int newPos = encoders[i].getPosition();
    if (newPos != *positions[i])
    {
      Serial.print("Encoder ");
      Serial.print(i);
      Serial.print(" position: ");
      Serial.print(newPos);
      Serial.print(" with direction ");
      Serial.println(newPos >= *positions[i]);
      Wire.beginTransmission(8);
      Serial.println("Transmission started");
      Wire.write(1);
      Serial.println("First byte sent");
      Wire.write(i);
      Wire.write((newPos >= *positions[i]) ? 1 : 0);
      Wire.endTransmission(); 
      Serial.println("I2C message sent");
      *positions[i] = newPos;
    }
  }
} // loop ()

// The End