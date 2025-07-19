#pragma once
#ifndef ENGINE_LEVEL_H
#define ENGINE_LEVEL_H

#define FILEPATH_BUFFER_SIZE 256

#include <vector>

#include "engine.h"
#include "engineMath.h"
#include "engineLevelQuad.h"
#include "engineRendererDrawBuffer.h"


class engineLevel : public module
{
    public:

    //EDITOR PROPERTIES...
    bool isInEditorMode;
    bool isParralaxEnabled;

    engineLevelQuad* highlightedQuadByCursor;
    engineLevelQuad* currentMenagedQuad;
    engineLevelQuad* currentToolTippedQuad;
    Vec2 highlightedQuadVertRealPositions[4];

    // TOOLTIP
    engineRenderDrawBuffer* chars;
    GLuint charsGLTexture;

    float tooltipAlpha;
    Vec2 tooltipPosition;
    char tooltipGuiStage;
    bool cursorAtTooltip;
    void manageTooltip();
    bool manageTooltipButton( Vec2 pos , Vec2 size , string text , float r , float g , float b);
    //bool manageTooltipSlidebar(  );
    //bool manageTooltipCheckbox(  );

    void tooltipSetQuadLogic();
    void tooltipManageQuadLogic();

    //tooltipSetQuadParallax(float mltpr);

    //---------
    void highlightQuadAtCursor();
    void tooltipQuadAtCursor();
    void selectQuadAtCursor();
    void dragSelectedQuad();
    void unselectQuad();

    Vec2 mouseCursorPrivPos;
    Vec2 mouseCursorCurentPos;
    Vec2 mouseCursorDifference;
    //--------------------


    vector<engineLevelQuad*> levelQuadVector;
    vector<engineLevelQuad*> levelQuadOnlyActiveVector;
    vector<engineLevelQuad*> levelQuadOnlyGroundVector;

    vector<engineLevelQuad*> levelQuadOnlyGroundVectorOutput;

    //vector<string>texturesLoadedIntoGame;
    vector<string>texturesInFolder;
    void fillTexturesData();


    //vector<string>levelTextureNamesVector;

    GLuint levelDefaultTexture;

    //engineRenderDrawBuffer* drawBuffer;

    engineLevel();
    ~engineLevel();

    void load();
    void synchro();
    void work();
    void draw();


    //---- SAVING / LOADING MAP -----

    bool pathEntered;
    bool isSaving;
    bool isLoading;

    string savePath;
    string loadPath;

    void menageIOFile();

    void loadLevel();
    void loadLevelShowWindow();

    void saveLevel();
    void saveLevelShowWindow();

    vector<engineLevelQuad*>getLevelGroundQuads();

};

engineLevel* M_getLevel();
#endif 