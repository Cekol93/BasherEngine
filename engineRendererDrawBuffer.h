#pragma once
#ifndef ENGINE_RENDER_DRAW_BUFFER_H
#define ENGINE_RENDER_DRAW_BUFFER_H

#include "engine.h"
#include "engineMath.h"

class engineRenderDrawBuffer
{
public:

    engineRenderDrawBuffer(int); //max_quads
    ~engineRenderDrawBuffer();


    void setColor(float r, float g, float b);
    void setAlpha(float a);
    //DRAWING METHODS:
    //void drawQuad(int Fr, Vec2 a, Vec2 b, Vec2 c, Vec2 d, int Col, int Row);
    //void drawText();
    void drawTile(int Fr, Vec2 Pos, Vec2 Size, float Ang, Vec2 Org, int Col, int Row);
    void drawQuadGeneric(Vec2 a, Vec2 b, Vec2 c, Vec2 d, Vec2 ta, Vec2 tb, Vec2 tc, Vec2 td);
    //CORE METHODS:
    void swapBuffers();
    void render(GLuint texture);
    //LIMITATIONS:


protected:



    int maxQuads;

    // Current drawing color
    float cR, cG, cB, cA;

    //REST OF SHIT:
    //GLuint GlTexture;
    int editedQuads;
    int drawedQuads;
    //ARRAYS:
    float* frontBufferV; // array
    float* frontBufferT; // array
    float* frontBufferC; // array
    float* backBufferV; // array
    float* backBufferT; // array
    float* backBufferC; // array
    //POINTERS:
    float* editedBufferV; // pointer
    float* editedBufferT; // pointer
    float* editedBufferC; // pointer
    float* drawedBufferV; // pointer
    float* drawedBufferT; // pointer
    float* drawedBufferC; // pointer
    
    bool visible;
};

#endif // ENGINE_RENDER_DRAW_BUFFER_H
