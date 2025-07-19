#include "engineKeyboard.h"
#include "engine.h"

engineKeyboard* localKeyboard = NULL;
engineKeyboard* M_getKeyboard() { return localKeyboard; }

void Keyboard(unsigned char key, int x, int y) {

    if (key > 92) key -= 32;
    localKeyboard->setKey(key);

}
void SKeyboard(int key, int x, int y) { localKeyboard->setKey(key); }
void KeyboardUp(unsigned char key, int x, int y) { if (key > 92) key -= 32; localKeyboard->resetKey(key); }
void SKeyboardUp(int key, int x, int y) { localKeyboard->resetKey(key); }

void engineKeyboard::setKey(unsigned int button) { keyOnuseIn[button] = 1; keyPressIn[button] = 1; }
void engineKeyboard::resetKey(unsigned int button) { keyOnuseIn[button] = 0; keyReleaseIn[button] = 1; }
bool engineKeyboard::isAnyKey() { return keyAny; }
bool engineKeyboard::key(unsigned int button) { if (typing) return 0; else return keyOnuseOut[button]; }
bool engineKeyboard::keyPressed(unsigned int button) { if (typing) return 0; return keyPressOut[button]; }
bool engineKeyboard::keyReleased(unsigned int button) { return keyReleaseOut[button]; }
bool engineKeyboard::isTyping() { return typing; }
void engineKeyboard::setTyping(string OldString) { cout << "ENTER! - Start of typing" << endl;  typing = 1;  for (int a = 0; a < keyString.length(); a++)  keyString.erase(a, 1); keyString.resize(0); keyString = OldString; }


engineKeyboard::engineKeyboard() {

    M_getEngine()->registerModule(this, "Engine Keyboard");
    localKeyboard = this;

    for (int i = 0; i < 256; i++)
    {
        keyOnuseOut[i] = keyOnuseIn[i] = 0;
        keyPressOut[i] = keyPressIn[i] = 0;
        keyReleaseOut[i] = keyReleaseIn[i] = 0;
    };
    keyAny = 0;
    lastKey = (char)0;
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUp);
    glutSpecialFunc(SKeyboard);
    glutSpecialUpFunc(SKeyboardUp);
}

engineKeyboard::~engineKeyboard() {
}

void engineKeyboard::synchro() {
    keyAny = 0;
    for (int i = 0; i < 256; i++)
    {
        keyOnuseOut[i] = keyOnuseIn[i];
        keyPressOut[i] = keyPressIn[i];
        keyReleaseOut[i] = keyReleaseIn[i];
        keyPressIn[i] = 0;
        keyReleaseIn[i] = 0;
        keyAny = max(keyAny, keyOnuseOut[i]);
        if (keyOnuseOut[i]) cout << i << endl;
    }

    if (keyPressOut[27])
        M_getEngine()->stop();

}

void engineKeyboard::work() {
    
    for (int i = 0; i < 256; i++)
    {
        if (keyPressOut[i]) lastKey = (char)i;
    }

    //debugMessage += "#0Last key number: " + to_string(lastKey);

}


void engineKeyboard::draw() {
}
