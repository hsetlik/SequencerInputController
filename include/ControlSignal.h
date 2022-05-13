#ifndef CONTROLSIGNAL_H
#define CONTROLSIGNAL_H
#include <Arduino.h>
//============PIN DEFINITIONS=====================
//Written for Arduino Nano as per KiCad schematic
#define ACL 0
#define ADA 1
//Button index : 0
#define ASW 2 

#define BCL 3
#define BDA 4
//Button index : 1
#define BSW 5

#define CCL 6
#define CDA 7
//Button index : 2
#define CSW 8

#define DCL 10
#define DDA 11
//Button index : 3
#define DSW 9

//Button index : 0
#define LBUTTON 14
//Button index : 0
#define RBUTTON 15

//Button index : 0
#define PLAY 16

//Button index : 0
#define TRK1 12
//Button index : 0
#define TRK2 17
//Button index : 0
#define TRK3 20
//Button index : 0
#define TRK4 21
//definitions for control signal bits
#define ISBUTTON_BIT 6
#define DIR_BIT 7
//==============================
enum ButtonId
{
    A,
    B,
    C,
    D,
    Left,
    Right,
    Trk1,
    Trk2,
    Trk3,
    Trk4,
    Play
};
const uint8_t allButtonPins[] = {ASW, BSW, CSW, DSW, LBUTTON, RBUTTON, TRK1, TRK2, TRK3, TRK4, PLAY };
struct ControlSignal
{
public:
    bool isButton;
    bool dir;
    uint8_t idx;
    byte asByte();
    static void printByte(byte bits);
 private:
    //basic bit manipulation functions
    static byte setBit(byte bits, uint8_t idx, bool value);
    static uint8_t byteIdx(byte bits);
    static bool bitValue(byte bits, uint8_t idx);
};



#endif