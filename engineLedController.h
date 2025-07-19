#pragma once
#ifndef ENGINE_LED_CONTROLLER_H
#define ENGINE_LED_CONTROLLER_H

#include "engine.h"
#include "engineRendererDrawBuffer.h"
#include "engineLedBlock.h"

class engineLedController : public module
{

//private:

public:
    vector<unsigned char>bytesBuffer;
    vector<engineLedBlock*>individualLeds;

    engineRenderDrawBuffer* drawBuffer;
    GLint GlFontTexture;

    //GLint GlRenderToTextureBig;
    //GLint GlRenderToTextureSmall;

    void prepareDrawContent();

//public:
  
    unsigned int editedParameter;
    unsigned int W, H;
    bool flipX,flipY;
    double R, G, B;


    bool playback;
    bool playbackLooped;
    double  playbackSpeed;
    double currentFrame;

    unsigned int totalFrames;
    unsigned int selectedFrame;
    bool deleteFrame;


    engineLedController();
    ~engineLedController();
    void load();
    void synchro();
    void work();
    void draw();

    void drawText(Vec2 pos, Vec2 scale, string text);
    void drawProgressBar(Vec2 pos, Vec2 scale, double progress);

    vector<engineLedBlock*> getLedBlocks();


};

engineLedController* M_getLedController();
#endif 