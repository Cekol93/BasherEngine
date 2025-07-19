
#ifndef ENGINE_MOUSE_H_INCLUDED
#define ENGINE_MOUSE_H_INCLUDED

#include "engine.h"
#include "engineMath.h"

class engineMouse : public module
{

private:

    bool buttonOnuseIn[5]; // Input shit
    bool buttonPressIn[5];
    bool buttonRelaceIn[5];

    bool buttonOnuseOut[5]; //Output (for threads) shit
    bool buttonPressOut[5];
    bool buttonRelaceOut[5];

    bool buttonAnyIn;
    bool motionAnyIn;

    bool buttonAnyOut;
    bool motionAnyOut;


    Vec2 rawPosOutput;
    Vec2 posOutput;

    Vec2 rawPosInput;
    Vec2 posInput;

    Vec2 rawPosHistory[2];
    float rawPosTime[2];

public:
    engineMouse();
    ~engineMouse();
    void draw();
    void synchro();
    void work();

    void setRawPos(int x, int y);

    void setButton(unsigned int button);
    void resetButton(unsigned int button);


    Vec2 getRawPosition();
    Vec2 getPosition();

    bool getMotionAny();
    bool getButtonAny();
    bool getButton(unsigned int button);
    bool getButtonPressed(unsigned int button);
    bool getButtonRelaced(unsigned int button);
    void resetInput();

};
engineMouse* M_getMouse();

#endif // JOYSTICK_H_INCLUDED
