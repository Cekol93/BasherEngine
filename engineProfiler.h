#pragma once
#ifndef ENGINE_PROFILER_H
#define ENGINE_PROFILER_H

#include "engine.h"
#include "engineMath.h"
#include "engineRendererDrawBuffer.h"

struct moduleInfo {
    module* M;
    long double executeTime;
};

struct coreInfo {
    int number;
    long double executeTime;
    int modules;
};

class engineProfiler : public module
{
public:
    engineProfiler();
    ~engineProfiler();
    void load();
    void synchro();
    void work();
    void draw();

    //FPS COUNTER;
    int drawFrames;
    int countedFrames;
    long double oneSecTimer;

    //MENU STUFF...
    bool active;

    void addModule(module* M, long double execute_time);// {  module_info Que = {M,execute_time}; Modules.push_back(Que);  }
    void addCore(int number, long double execute_time, int modules);// {  core_info Que = {number,execute_time}; Modules.push_back(Que);  }
    //DRAWING STUFF...
    void drawText(Vec2 pos, Vec2 scale, string text);
    void drawBar(Vec2 start, Vec2 end, float state, float maxstate);
    //float *TexCoords; // preloaded texture charset

    engineRenderDrawBuffer* chars;

    int intakeModulesCount; //
    int intakeCoresCount; //
    int outtakeModulesCount; //
    int outtakeCoresCount; //
    int outtakeCoresCountSmooth;
    //ARRAYS:
    moduleInfo* frontModules; // array
    moduleInfo* backModules; // array
    coreInfo* frontCores; // array
    coreInfo* backCores; // array
    coreInfo* backCoresSmooth; // array
    //POINTERS:
    moduleInfo* intakeModules; // pointer
    moduleInfo* outtakeModules; // pointer
    coreInfo* intakeCores; // pointer
    coreInfo* outtakeCores; // pointer

    long double renderDtSmooth;
    long double engineDtSmooth;
    long double smoothFactor;

    bool visible;
    GLuint GlTexture;

};
engineProfiler* M_getProfiler();
#endif // PROFILER_H
