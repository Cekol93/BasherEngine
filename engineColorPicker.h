#pragma once

#ifndef ENGINE_COLOR_PICKER_H
#define ENGINE_COLOR_PICKER_H

#include "engine.h"
#include "engineRendererDrawBuffer.h"

class engineColorPicker : public module
{
protected:
    double pickedRedFrontBuffer, pickedGreenFrontBuffer, pickedBlueFrontBuffer;
    double pickedRedBackBuffer, pickedGreenBackBuffer, pickedBlueBackBuffer;

    bool openedFrontBuffer;
    bool openedBackBuffer;
    
    engineRenderDrawBuffer* drawBuffer;
    GLuint glFontTexture;

    void drawText(Vec2 pos, Vec2 scale, string text);

public:
    engineColorPicker();
    ~engineColorPicker();
    void load();
    void synchro();
    void work();
    void draw();

    double getR();
    double getG();
    double getB();
    bool isOpened();
};

engineColorPicker* M_getColorPicker();
#endif 