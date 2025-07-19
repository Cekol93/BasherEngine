#pragma once
#ifndef ENGINE_KEYBOARD_H_INCLUDED
#define ENGINE_KEYBOARD_H_INCLUDED

#include "engine.h"


class engineKeyboard : public module
{

private:

    bool keyOnuseIn[256]; // Input shit
    bool keyPressIn[256];
    bool keyReleaseIn[256];

    bool keyOnuseOut[256]; //Output (for threads) shit
    bool keyPressOut[256];
    bool keyReleaseOut[256];
    bool keyAny;
    char lastKey;


    string keyString = "";
    bool typing = 0;



public:
    engineKeyboard();
    ~engineKeyboard();
    void draw();
    void synchro();
    void work();

    void setKey(unsigned int button);
    void resetKey(unsigned int button);

    bool isAnyKey();
    bool key(unsigned int button);
    bool keyPressed(unsigned int button);
    bool keyReleased(unsigned int button);
    bool isTyping();
    void setTyping(string OldString);
    void resetInput();

};
engineKeyboard* M_getKeyboard();

#endif // KEYBOARD_H_INCLUDED
