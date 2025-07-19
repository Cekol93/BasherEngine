#pragma once
#ifndef ENGINE_LEVEL_QUAD_DECAL_H
#define ENGINE_LEVEL_QUAD_DECAL_H

#include "engine.h"
#include "engineMath.h"
#include "engineRenderer.h"
#include "engineRendererDrawBuffer.h"




class engineLevelQuadDecal : public module
{
public:

    Vec2 decalRangePositions[4];
    Vec2 positionOnLevel;

    float angle;
    Vec2 scale;
    Vec2 offset;

    engineRenderDrawBuffer* drawBuffer;
    GLuint glTexture;

    engineLevelQuadDecal();
    ~engineLevelQuadDecal();
    void load();
    void synchro();
    void work();
    void draw();

    Vec2 getTextureCoOrdinates(Vec2 VecPos);

};


#endif 