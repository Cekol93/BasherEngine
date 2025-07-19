#pragma once
#ifndef ENGINE_LEVEL_QUAD_H
#define ENGINE_LEVEL_QUAD_H

#include "engine.h"
#include "engineMath.h"
#include "enginePhysics.h"

class engineLevelQuad : public module
{

public:

    bool quadIsActive;
    bool quadIsActiveBackBuffer; // For output...

    bool quadIsInEditorMode;
    bool quadLoadingFailed;



    //Texture Crap...
    //Vec2 quadTexCenterPos;
    //Vec2 quadTexTopPos;       ???
    //Vec2 quadTexBotPos;

    GLuint quadTextureID;
    string quadTextureName;

    // Positioning Crap...
    Vec2 quadTexCoords[4];            //4 texture positions
    Vec2 quadVertRealPositions[4];    //4 real (not modify by paralax) vertex points...
    Vec2 quadVertVisiblePositions[4]; //4 visible (modyfied by parallax) vertex points...


    Vec2 quadCenterPoint;
    Vec2 quadVisibleCenterPoint;
    double quadLongestDistanceInside; //Naj dystans od srodka do rogu

    //Quad logic / job

    unsigned long int quadUniqueID;

    double quadSortID;

    char quadLogicMode; // 0 - GROUND  1-PARALAX
    double quadParralaxAmount;
    bool isParralaxEnabled;


    //Editor Crap...
    bool quadIsSelectedInEditor;
    char quadSelectedPart;

    //
    bool quadIsToDestroy;

    enginePhysicsObject* physicsBody;
    void managePhysicBody();

    mutex quadDataLock;

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
    void swapDrawingBuffer();

    /**/
    engineLevelQuad();
    ~engineLevelQuad();
    void load();
    void synchro();
    void work();
    void draw();

    
    bool isPointInside(Vec2 point);

    void quadEditSelectPart(Vec2 point);
    void quadEdit(Vec2 moveVector);
    void quadEditEnd();

    void quadSwapToGrid();
    void quadSetVertexPositions(float, float, float, float, float, float, float, float);                // ustawnianie pozycji vectorow
    void quadSetTexureCoords(float, float, float, float, float, float, float, float);             // ustawianie pozycji tekstur
    void manageTexture();

    Vec2 getNearrestPointInside(Vec2 point);

    //-------------------------------------
    void RotareVertexes();
    void RotareTexCoords();

    void Copy();
    void PrepareAfterLoading();

    void ForceToGrid(int);

    bool DoIncludePoint(Vec2&);

    void Select_Part(Vec2&);

    void Editor_Properties();

    void Move_Part(Vec2&);
    //--
    void calculatePositions();

    void fillDrawingBuffer();


    //---
    void WriteToFile();
    void Mapfile_Transfer_Data();

    //void Step();
    void PhysStep();

    void Draw();
    void EditorDraw();

    float Get_QuadWallangle(Vec2 POINT);
    float Get_QuadWalllenght(Vec2 POINT);
};
#endif 