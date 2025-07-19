#pragma once
#ifndef ENGINE_DEBUG_INFO_H
#define ENGINE_DEBUG_INFO_H

#include "engine.h"
#include "engineMath.h"
#include "engineRendererDrawBuffer.h"

class engineDebugInfo : public module
{
public:
    engineDebugInfo();
    ~engineDebugInfo();
    void load();
    void synchro();
    void work();
    void draw();
    void addDebugMessage(string msg);
    mutex m;

    string inputDebugInfo;
    string outputDebugInfo;


    //MENU STUFF...
    bool active;

    //DRAWING STUFF...
    void drawText(Vec2 pos, Vec2 scale, string text);
    void drawBar(Vec2 start, Vec2 end, float state, float maxstate);
    string boolStatus(bool status);

    //float *TexCoords; // preloaded texture charset

    engineRenderDrawBuffer* chars;

    bool visible;
    GLuint GlTexture;

};
engineDebugInfo* M_getDebugInfo();


#endif // PROFILER_H
