
#include "engine.h"
#include "engineMouse.h"
#include "engineRenderer.h"

engineMouse* localMouse = NULL;
engineMouse* M_getMouse() { return localMouse; }

void MouseMotion(int x, int y) {
    localMouse->setRawPos(x, y);
}

void MouseButton(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) localMouse->setButton(button);
    if (state == GLUT_UP)   localMouse->resetButton(button);
    localMouse->setRawPos(x, y);
}
void engineMouse::setButton(unsigned int button) { buttonOnuseIn[button] = 1;  buttonPressIn[button] = 1; }
void engineMouse::resetButton(unsigned int button) { buttonOnuseIn[button] = 0; buttonRelaceIn[button] = 1; }

void engineMouse::setRawPos(int x, int y) {


    for (int i = 1; i >= 0; i--)
    {
        rawPosHistory[i] = rawPosHistory[i - 1];
        rawPosTime[i] = rawPosTime[i - 1];
    }
    rawPosHistory[0].Set(x, y);
    rawPosTime[0] = getTime();


}

Vec2 engineMouse::getRawPosition() { return rawPosOutput; }
Vec2 engineMouse::getPosition() { return posOutput; }
bool engineMouse::getMotionAny() { return motionAnyOut; }
bool engineMouse::getButtonAny() { return buttonAnyOut; }
bool engineMouse::getButton(unsigned int button) { return buttonOnuseOut[button]; }
bool engineMouse::getButtonPressed(unsigned int button) { return buttonPressOut[button]; }
bool engineMouse::getButtonRelaced(unsigned int button) { return buttonRelaceOut[button]; }




engineMouse::engineMouse() {

    M_getEngine()->registerModule(this, "Engine Mouse");
    localMouse = this;

    for (int i = 0; i < 5; i++)
    {
        buttonOnuseOut[i] = buttonOnuseIn[i] = 0;
        buttonPressOut[i] = buttonPressIn[i] = 0;
        buttonRelaceOut[i] = buttonRelaceIn[i] = 0;
    }

    for (int i = 0; i < 2; i++)
    {
        rawPosHistory[i].Clear();
        rawPosTime[i] = getTime();
    }
    rawPosInput.Clear();
    rawPosOutput.Clear();
    posInput.Clear();
    posOutput.Clear();

    buttonAnyIn = 0;
    motionAnyIn = 0;

    buttonAnyOut = 0;
    motionAnyOut = 0;

    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(MouseMotion);
    glutMouseFunc(MouseButton);

    //glutSetCursor(GLUT_CURSOR_NONE);

}

engineMouse::~engineMouse() {
}

void engineMouse::synchro() {

    rawPosOutput = rawPosInput;
    posOutput = posInput;

    buttonAnyIn = 0;
    for (int i = 0; i < 5; i++)
    {
        //cout<<"MOUSE "<<i<<" B:"<<Button_Onuse_In[i]<<"/"<<Button_Press_In[i]<<"/"<<Button_Relace_In[i]<<endl;
        buttonOnuseOut[i] = buttonOnuseIn[i];
        buttonPressOut[i] = buttonPressIn[i];
        buttonRelaceOut[i] = buttonRelaceIn[i];
        buttonAnyIn = max(buttonAnyIn, buttonOnuseOut[i]);

        buttonPressIn[i] = 0;
        buttonRelaceIn[i] = 0;
    }
    buttonAnyOut = buttonAnyIn;
    motionAnyOut = motionAnyIn;
    motionAnyIn = 0;
    buttonAnyIn = 0;


}

void engineMouse::work() {

    float Progress = (getTime() - rawPosTime[0]);
    float Latency = max(0.0f, rawPosTime[0] - rawPosTime[1]);
    Progress = min(1.000f, max(0.000f,(float)Progress / (float)Latency));
    rawPosInput = rawPosHistory[1] + (Progress) * (rawPosHistory[0] - rawPosHistory[1]);  // BASIC  POSITION

    if (Progress < 1.0f) motionAnyIn = 1; else motionAnyIn = 0;
    //debugMessage += to_string(Progress) + "/x:" + to_string(rawPosInput.x) + " /y: " + to_string(rawPosInput.y);

    posInput.Set( M_getRender()->get_position_x(), M_getRender()->get_position_y() );

    posInput +=
        lengthdir(((rawPosInput.x - M_getRender()->getWindowsizeW() / 2) / M_getRender()->getWindowsizeW())
            * M_getRender()->get_resolution_w() * (1 / M_getRender()->get_scale_w()), M_getRender()->get_angle());

    posInput +=
        lengthdir(((rawPosInput.y - M_getRender()->getWindowsizeH() / 2) / M_getRender()->getWindowsizeH())
            * M_getRender()->get_resolution_h() * (1 / M_getRender()->get_scale_h()), M_getRender()->get_angle() - 90);

}

void engineMouse::draw() {
}
