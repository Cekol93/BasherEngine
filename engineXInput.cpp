#include "engineXInput.h"
#include "engine.h"

// <climits>

engineXInput* localXInput = NULL;
engineXInput* M_getXInput() { return localXInput; }

engineXInput::engineXInput() {

    M_getEngine()->registerModule(this, "Engine X Input");
    localXInput = this;

    unsigned int p = 0; // Clear data!
    for (p = 0; p < 192; p++)
        dataOutBool[p] = dataInBool[p] = false;
    for (p = 0; p < 32; p++)
        dataOutFloat[p] = dataInFloat[p] = 0.0f;

    ZeroMemory(&xInputState, sizeof(XINPUT_STATE)*4 );
    ZeroMemory(&xInputVibration, sizeof(XINPUT_VIBRATION)*4 );

}

engineXInput::~engineXInput() {
}

void engineXInput::synchro() {

    unsigned int p = 0;

    for (p = 0; p < 192; p++)
        dataOutBool[p] = dataInBool[p];

    for (p = 0; p < 32; p++)
        dataOutFloat[p] = dataInFloat[p];
}

void engineXInput::work() {

    for (char i = 0; i < 4; i++) { // 4 CONTROLLERS

        bool dataBool[16];
        for (char i = 0; i < 16; i++)
            dataBool[i] = false;

        unsigned int dataOffset;

        if (XInputGetState(i, &xInputState[i]) == ERROR_SUCCESS)
        {
            // ANALOG AXEL DATA
            SHORT sThumbLX = (xInputState[i].Gamepad.sThumbLX);
            SHORT sThumbLY = (xInputState[i].Gamepad.sThumbLY);
            SHORT sThumbRX = (xInputState[i].Gamepad.sThumbRX);
            SHORT sThumbRY = (xInputState[i].Gamepad.sThumbRY);
            BYTE bLeftTrigger = (xInputState[i].Gamepad.bLeftTrigger);
            BYTE bRightTrigger = (xInputState[i].Gamepad.bRightTrigger);

            dataOffset = i * 8;
            dataInFloat[0 + dataOffset] = max(-1.0f, min(1.0f, static_cast<float>(sThumbLX) * 0.0000305185f));
            dataInFloat[1 + dataOffset] = max(-1.0f, min(1.0f, static_cast<float>(sThumbLY) * 0.0000305185f));
            dataInFloat[2 + dataOffset] = max(-1.0f, min(1.0f, static_cast<float>(sThumbRX) * 0.0000305185f));
            dataInFloat[3 + dataOffset] = max(-1.0f, min(1.0f, static_cast<float>(sThumbRY) * 0.0000305185f));
            dataInFloat[4 + dataOffset] = max(-1.0f, min(1.0f, static_cast<float>(sThumbLX) * 0.0000305185f));
            dataInFloat[5 + dataOffset] = max(-1.0f, min(1.0f, static_cast<float>(sThumbLX) * 0.0000305185f));
            dataInFloat[6 + dataOffset] = max(0.0f, min(1.0f, static_cast<float>(bLeftTrigger) * 0.00392156862f));
            dataInFloat[7 + dataOffset] = max(0.0f, min(1.0f, static_cast<float>(bRightTrigger) * 0.00392156862f));

            //DIGITAL BUTTONS DATA
            dataBool[0] = true; // 0 place is for "is gamepad connected"
            dataBool[1] = false;// 1 place is for "is gamepad pressed any key"
            dataBool[2] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
            dataBool[3] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
            dataBool[4] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
            dataBool[5] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
            dataBool[6] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
            dataBool[7] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
            dataBool[8] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
            dataBool[9] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
            dataBool[10] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
            dataBool[11] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
            dataBool[12] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
            dataBool[13] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
            dataBool[14] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
            dataBool[15] = (xInputState[i].Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;

            for (char p = 2; p < 16; p++)
                dataBool[1] = max(dataBool[1], dataBool[p]);
        }

        //DATA RANGE: | 0-15 Input | 16-31 Key Pressed | 32-47 Key Released |

        dataOffset = i * 48;
        for (char p = 0; p < 16; p++) {

            dataInBool[p + 16 + dataOffset] = (dataBool[p] == true && dataInBool[p + dataOffset] == false); // Key just Pressed!
            dataInBool[p + 32 + dataOffset] = (dataBool[p] == false && dataInBool[p + dataOffset] == true); // Key just Released!
            dataInBool[p + dataOffset] = dataBool[p]; // Normal Button
        }

    }

    // DEBUG INFO...
    int gamePadsConnected = 0;
    for (int i = 0; i < 4; i++)
        if (dataInBool[0 + i * 48]) gamePadsConnected += 1;

    if (gamePadsConnected == 0)
        debugMessage += "#3No connected GamePads";
    else {
        debugMessage += "#6" + to_string(gamePadsConnected)+ " connected GamePads:#n";
        for (int i = 0; i < gamePadsConnected; i++) {
            debugMessage += " #0" + to_string(i) + " Buttons status: ";
            for (int j = 0; j < 48; j++) {
                if (j == 0)  debugMessage += " #0B:";
                if (j == 16 ) debugMessage += " #0P:";
                if (j == 32)  debugMessage += " #0R:";
                if (dataInBool[j + i * 48]) debugMessage += "#4|";
                else debugMessage += "#1O";
            }
            debugMessage += "#n";
        }
    }
}

void engineXInput::draw() {
}

float engineXInput::axis( unsigned int gamePad, unsigned int axisID) { return dataOutFloat[ max((unsigned int)0, min((unsigned int)3, (unsigned int)gamePad ))*8 + axisID ]; }
bool engineXInput::button (unsigned int gamePad, unsigned int buttonID ) { return dataOutBool[max((unsigned int)0, min((unsigned int)3, (unsigned int)gamePad)) * 48 + buttonID];}
bool engineXInput::buttonPressed(unsigned int gamePad, unsigned int buttonID) { return dataOutBool[max((unsigned int)0, min((unsigned int)3, (unsigned int)gamePad)) * 48 + buttonID + 16]; }
bool engineXInput::buttonReleased( unsigned int gamePad, unsigned int buttonID ) { return dataOutBool[max((unsigned int)0, min((unsigned int)3, (unsigned int)gamePad)) * 48 + buttonID + 32]; }
