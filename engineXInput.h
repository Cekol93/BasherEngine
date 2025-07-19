#pragma once
#ifndef ENGINE_XINPUT_H_INCLUDED
#define ENGINE_XINPUT_H_INCLUDED

#define NOMINMAX

#include "engine.h"

#include <Xinput.h>

/*
IS_CONNECTED
XINPUT_GAMEPAD_START
XINPUT_GAMEPAD_SELECT
XINPUT_GAMEPAD_A
XINPUT_GAMEPAD_B
XINPUT_GAMEPAD_X
XINPUT_GAMEPAD_Y
XINPUT_GAMEPAD_DPAD_LEFT
XINPUT_GAMEPAD_DPAD_RIGHT
XINPUT_GAMEPAD_DPAD_UP
XINPUT_GAMEPAD_DPAD_DOWN
XINPUT_GAMEPAD_LEFT_SHOULDER
XINPUT_GAMEPAD_RIGHT_SHOULDER
XINPUT_GAMEPAD_LEFT_THUMB
XINPUT_GAMEPAD_RIGHT_THUMB


16 DIGITAL BUTTONS MAX X 3 (Input/Press/Release) X 4 CONTROLLERS = 192 Bolean

*/

//
//8  ANALOG AXES MAX X 4 CONTROLLERS = 32 Floats

class engineXInput : public module
{

private:

    XINPUT_STATE xInputState[4];
    XINPUT_VIBRATION xInputVibration[4];

    bool  dataInBool[192]; // Input bool shit
    float dataInFloat[32]; // Input float shit

    bool  dataOutBool[192]; // Output bool shit
    float dataOutFloat[32]; // Output float shit

public:
    engineXInput();
    ~engineXInput();
    void draw();
    void synchro();
    void work();


    float axis(unsigned int gamePad, unsigned int axisID);
    bool button( unsigned int gamePad , unsigned int buttonID );
    bool buttonPressed( unsigned int gamePad , unsigned int buttonID);
    bool buttonReleased( unsigned int gamePad , unsigned int buttonID);


};
engineXInput* M_getXInput();

#endif // ENGINE_XINPUT_H_INCLUDED
