#include "engineLevel.h"
#include "engineLevelQuad.h"


#include "engine.h"
#include "engineRenderer.h"
#include "engineRendererDrawBuffer.h"

#include "engineKeyboard.h"
#include "engineMouse.h"


#include <windows.h>
#include <stack>
#include <shobjidl.h> 
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>



engineLevel* localLevel = NULL;
engineLevel* M_getLevel() { return localLevel; }


engineLevel::engineLevel()
{
    M_getEngine()->registerModule(this, "Engine Level");
    localLevel = this;

    levelQuadVector.clear();

    isInEditorMode = true;
    isParralaxEnabled = true;

    currentMenagedQuad = nullptr;
    currentToolTippedQuad = nullptr;

    charsGLTexture = NULL;
    chars = new engineRenderDrawBuffer(4096);
    texturesInFolder.clear();

    mouseCursorPrivPos.Clear();
    mouseCursorCurentPos.Clear();
    mouseCursorDifference.Clear();
}
engineLevel::~engineLevel() {

}

class levelQuadVectorSortClass {
public:
    bool operator () (const engineLevelQuad* e1, const  engineLevelQuad* e2)
    {
        //return (q1->SortID < q2->SortID) ? true : false;
        return (e1->quadSortID < e2->quadSortID) ? false : true;
    }

};

void engineLevel::load() {

    levelDefaultTexture = M_getRender()->get_texture("myQuad.png");
    charsGLTexture = M_getRender()->get_texture("menu_font.png");

    fillTexturesData();
}
void engineLevel::synchro() {

    levelQuadOnlyGroundVectorOutput = levelQuadOnlyGroundVector;
    //Tooltip...
    if (!isInEditorMode) return;
    chars->swapBuffers();

    if (M_getRender()) 
        M_getRender()->add_module(this, -1000);

}
void engineLevel::work() {
    
    // Deleting quads...
    for (int i = 0; i < levelQuadVector.size(); i++) {
        engineLevelQuad* q = levelQuadVector[i];
        if (q->quadIsToDestroy) {
            q->quadIsActive = false;
            q->moduleToDestroy = true;
            levelQuadVector.erase(levelQuadVector.begin() + i);
        }
    }

    std::sort(levelQuadVector.begin(), levelQuadVector.end(), levelQuadVectorSortClass());


    levelQuadOnlyActiveVector.clear();
    for (int i = 0; i < levelQuadVector.size(); i++) {
        engineLevelQuad* q = levelQuadVector[i];
        if (q->quadIsActiveBackBuffer)
            levelQuadOnlyActiveVector.push_back(q);
    }


    levelQuadOnlyGroundVector.clear();
    for (int i = 0; i < levelQuadOnlyActiveVector.size(); i++) {
        engineLevelQuad* q = levelQuadOnlyActiveVector[i];
        if (q->quadLogicMode == 0)
            levelQuadOnlyGroundVector.push_back(q);
    }

    debugMessage += "Total blocks:" + to_string(levelQuadVector.size());
    debugMessage += " Active:" + to_string(levelQuadOnlyActiveVector.size());

    if (!M_getMouse() || !M_getKeyboard()) return;

    

    mouseCursorPrivPos = mouseCursorCurentPos;
    mouseCursorCurentPos = M_getMouse()->getPosition();
    mouseCursorDifference = (mouseCursorCurentPos - mouseCursorPrivPos);

    //cout << mouseCursorDifference.x << "/" << mouseCursorDifference.y << endl;

    //if (M_getKeyboard()->keyPressed(32)); //isInEditorMode = !isInEditorMode;

    if (M_getKeyboard()->keyPressed(106)) { // INSERT KEY

        engineLevelQuad* newMapQuad = new engineLevelQuad();
        float mX = M_getMouse()->getPosition().x;
        float mY = M_getMouse()->getPosition().y;
        float quadSize = 256.0f;
        newMapQuad->quadSetVertexPositions
        (
            mX - quadSize, mY - quadSize
            , mX + quadSize, mY - quadSize
            , mX + quadSize, mY + quadSize
            , mX - quadSize, mY + quadSize
        );
        newMapQuad->quadSetTexureCoords
        (
            0.0f, 0.0f
            , 1.0f, 0.0f
            , 1.0f, 1.0f
            , 0.0f, 1.0f
        );
        newMapQuad->quadUniqueID = levelQuadVector.size() + 1;
        newMapQuad->quadTextureName = "myQuad.png";
        levelQuadVector.push_back(newMapQuad);
    }

    if (M_getMouse()->getButtonPressed(1))
        M_getRender()->set_position(M_getMouse()->getPosition().x, M_getMouse()->getPosition().y);

    //if (M_getMouse()->getButton(1)) {
    if (M_getMouse()->getButtonPressed(3)) {
        M_getRender()->set_scale(M_getRender()->get_scale_w() * 2.0f, M_getRender()->get_scale_h() * 2.0f);
        M_getRender()->set_position(M_getMouse()->getPosition().x, M_getMouse()->getPosition().y);
    }

    if (M_getMouse()->getButtonPressed(4))
        M_getRender()->set_scale(M_getRender()->get_scale_w() * 0.5f, M_getRender()->get_scale_h() * 0.5f);
    //}

    highlightQuadAtCursor();

    if (M_getMouse()->getButtonPressed(2)) tooltipQuadAtCursor();

    if ( M_getMouse()->getButtonPressed(0)) selectQuadAtCursor();
    if ( M_getMouse()->getButton(0) ) dragSelectedQuad();
    if ( M_getMouse()->getButtonRelaced(0) ) unselectQuad();
    
    
    

    for (int i = 0; i < levelQuadVector.size(); i++) {
        levelQuadVector[i]->isParralaxEnabled = this->isParralaxEnabled;
        levelQuadVector[i]->quadIsInEditorMode = this->isInEditorMode;
    }


    if (highlightedQuadByCursor != nullptr) { // ADD QUAD EFFECT ON HIGHLIGHTED QUAD
        cout << highlightedQuadByCursor << endl;
        highlightedQuadByCursor->quadDataLock.lock();
        for (int i = 0; i < 4; i++)
            highlightedQuadVertRealPositions[i] = highlightedQuadByCursor->quadVertRealPositions[i];
        highlightedQuadByCursor->quadDataLock.unlock();

        chars->setAlpha(1.0f);
        chars->setColor(1.0f, 1.0f, 1.0f);
        float part = 1.0f / 16.0f;
        chars->drawQuadGeneric(
            highlightedQuadVertRealPositions[0],
            highlightedQuadVertRealPositions[1],
            highlightedQuadVertRealPositions[2],
            highlightedQuadVertRealPositions[3],
            Vec2(0.0f, 0.0f),
            Vec2(part, 0.0f),
            Vec2(part, part),
            Vec2(0.0f, part)
        );


    }

    manageTooltip();
    menageIOFile();

}
void engineLevel::draw() {
    if (chars)
        chars->render(charsGLTexture);

}

void engineLevel::highlightQuadAtCursor() {

    if (currentToolTippedQuad) return;

    Vec2 mousePos = M_getMouse()->getPosition();

    highlightedQuadByCursor = nullptr;
    for (int i = 0; i < levelQuadVector.size(); i++) {
        engineLevelQuad* q = (engineLevelQuad*)levelQuadVector[i];
        if (q->isPointInside(mousePos)) {
            highlightedQuadByCursor = q;
        }
    }

}
void engineLevel::tooltipQuadAtCursor() {

    unselectQuad();
    if (currentToolTippedQuad) {
        currentToolTippedQuad = nullptr;
        return;
    }

    tooltipAlpha = 0.0f;
    tooltipPosition = M_getMouse()->getPosition();

    if (highlightedQuadByCursor)
        currentToolTippedQuad = highlightedQuadByCursor;
    /*
    for (int i = 0; i < levelQuadVector.size(); i++) {
        engineLevelQuad* q = (engineLevelQuad*)levelQuadVector[i];
        if (q->isPointInside(tooltipPosition)) {
            currentToolTippedQuad = q;
            return;
        }
    }*/

}
void engineLevel::selectQuadAtCursor() {

    if (cursorAtTooltip) return;

    currentToolTippedQuad = nullptr;

    Vec2 curPos = M_getMouse()->getPosition();

    if (highlightedQuadByCursor) {
        currentMenagedQuad = highlightedQuadByCursor;
        currentMenagedQuad->quadEditSelectPart(curPos);
    }

    /*for (int i = 0; i < levelQuadVector.size(); i++) {
        engineLevelQuad* q = (engineLevelQuad*)levelQuadVector[i];
        if (q->isPointInside(curPos)) {
            currentMenagedQuad = q;
            currentMenagedQuad -> quadEditSelectPart(curPos);
            cout << "Quad selected " << q << endl;
            return;
        }
    }*/

}

void engineLevel::dragSelectedQuad() {
    if (currentMenagedQuad) {
        currentMenagedQuad->quadEdit(mouseCursorDifference);
        highlightedQuadByCursor = currentMenagedQuad;
    }
}
void engineLevel::unselectQuad() {
    if (currentMenagedQuad) {
        currentMenagedQuad -> quadEditEnd();
    }
    currentMenagedQuad = nullptr;    
}


void engineLevel::manageTooltip() {
    if (!currentToolTippedQuad) {
        tooltipGuiStage = 0;
        tooltipAlpha = 0.0f;
        cursorAtTooltip = false;
    }
    else {
        tooltipAlpha += min(1.0f , max (0.0f , 0.01f * (float)deltaTime));
        cursorAtTooltip = false; // This goes true in tooltipButtons

        if (tooltipGuiStage == 0) { // Main Tooltip Gui Menu
            if (manageTooltipButton(Vec2(0, 0), Vec2(1, 1), "Set Quad Logic", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 1; return; }
            if (manageTooltipButton(Vec2(0, 64), Vec2(1, 1), "Logic Properties", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 2; return; }
            if (manageTooltipButton(Vec2(0, 128), Vec2(1, 1), "Texture Properties", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 3; return; }
            if (manageTooltipButton(Vec2(0, 192), Vec2(1, 1), "Clone", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 4; return; }
            if (manageTooltipButton(Vec2(0, 256), Vec2(1, 1), "Remove from scene", 0.9f, 0.2, 0.2f)) {
                
                currentToolTippedQuad->quadIsToDestroy = true;
                currentToolTippedQuad = nullptr;
                return;
            
            }
        }
        if (tooltipGuiStage == 1) {// Set Quad logic 
            tooltipSetQuadLogic();
            return;
        }
        if (tooltipGuiStage == 2) {// Quad logic tooltip
            tooltipManageQuadLogic();
            return;
        }
        if (tooltipGuiStage == 3) {// Texture tooltip
            if (manageTooltipButton(Vec2(0, 0), Vec2(1, 1), "Set new Texture", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 4; return; }
            if (manageTooltipButton(Vec2(0, 64), Vec2(1, 1), "Modify TextCords", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 0; return; }
            //if (manageTooltipButton(Vec2(0, 128), Vec2(1, 1), "Texture Properties", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 3; return; }
            //if (manageTooltipButton(Vec2(0, 192), Vec2(1, 1), "Clone", 1.0f, 1.0f, 1.0f)) { tooltipGuiStage = 4; return; }
            //if (manageTooltipButton(Vec2(0, 256), Vec2(1, 1), "Remove from scene", 0.9f, 0.2, 0.2f)) { tooltipGuiStage = 5; return; }
        }
        if (tooltipGuiStage == 4) {

            for (int i = 0 ; i< texturesInFolder.size() ; i++)
                if (manageTooltipButton(Vec2(0, 0 +i*64 ), Vec2(1, 1), texturesInFolder[i] , 1.0f, 1.0f, 1.0f)) { 
                    currentToolTippedQuad->quadTextureID = NULL;
                    currentToolTippedQuad->quadTextureName = texturesInFolder[i];
                    tooltipGuiStage = 3; return; 
                }


        }
    }
}

bool engineLevel::manageTooltipButton(Vec2 pos, Vec2 size, string text, float r, float g, float b) {
    Vec2 Pos = tooltipPosition + pos ; //Vec2(pos.x - (+64 + str.length()*64)*scale.x*0.5f ,pos.y);
    float buttonLen = 25.0f;
    bool buttonHighlighted = false;

    if (M_getMouse()->getPosition().x > Pos.x && M_getMouse()->getPosition().x < Pos.x+ (64 * size.x * buttonLen) )
        if (M_getMouse()->getPosition().y > Pos.y - (32.0f* size.y)  && M_getMouse()->getPosition().y < Pos.y + 32 * size.y )
            buttonHighlighted = true;
    float colAmp = 1.0f;

    if (buttonHighlighted) cursorAtTooltip = true;

    chars->setAlpha(tooltipAlpha);

    //colAmp = 0.25f + buttonHighlighted * 0.75f;
    chars->setColor(0.2,0.2,0.2);

    chars->drawTile(max(0, min((int)11, 255)), Pos - Vec2(64.0f,0.0f) , Vec2(64 * size.x * buttonLen, 64 * size.y), 0, Vec2(0.0, 0.5), 16, 16);

    //chars->setAlpha(1.0f);
    colAmp = 0.5f + buttonHighlighted * 0.5f;
    chars->setColor(r * colAmp, g * colAmp, b * colAmp);

    for (std::string::iterator c = text.begin(); c != text.end(); ++c) {
        Pos.x += 55 * size.x;
        chars->drawTile(max(0, min((int)*c, 255)), Pos, Vec2(50 * size.x, 50 * size.y), 0, Vec2(0.5, 0.5), 16, 16);
    }
    bool bPressed = (buttonHighlighted && M_getMouse()->getButtonPressed(0));
    if (bPressed) tooltipAlpha = 0.25f;
    return bPressed;
}
/*
bool engineLevel::manageTooltipSlidebar() {

}
bool engineLevel::manageTooltipCheckbox() {

}*/

void engineLevel::tooltipSetQuadLogic() {


    if (!currentToolTippedQuad) return;
    bool cLg = false;
    
    //;
    cLg = (currentToolTippedQuad->quadLogicMode == 0);
    if (manageTooltipButton(Vec2(0, 0), Vec2(1, 1), "Set As Ground", 0.75f, 0.75f + 0.25f*(float)cLg, 0.75f )) { 
        currentToolTippedQuad->quadLogicMode = 0;
        currentToolTippedQuad = nullptr;
        return;
    }
    
    cLg = (currentToolTippedQuad->quadLogicMode == 1);
    if (manageTooltipButton(Vec2(0, 64), Vec2(1, 1), "Set As Paralax", 0.75f, 0.75f + 0.25f * (float)cLg, 0.75f )) {
        currentToolTippedQuad->quadLogicMode = 1;
        currentToolTippedQuad = nullptr;
        return;
    }
    
    cLg = (currentToolTippedQuad->quadLogicMode == 10);
    if (manageTooltipButton(Vec2(0, 128), Vec2(1, 1), "Set As Background", 0.75f, 0.75f + 0.25f * (float)cLg, 0.75f )) {
        currentToolTippedQuad->quadLogicMode = 10;
        currentToolTippedQuad = nullptr;
        return;
    }

}
void engineLevel::tooltipManageQuadLogic() {
    if (!currentToolTippedQuad) return;

 
    
    if (currentToolTippedQuad->quadLogicMode == 1) { // is our quad a ground?!
        if (manageTooltipButton(Vec2(0, 0), Vec2(1, 1), "Parallax +0.1", 1.0f, 1.0f, 1.0f)) {
            currentToolTippedQuad->quadParralaxAmount += 0.1f;
            return;
        }

        if (manageTooltipButton(Vec2(0, 64), Vec2(1, 1), "Parallax -0.1", 1.0f, 1.0f, 1.0f)) {
            currentToolTippedQuad->quadParralaxAmount -= 0.1f;
            return;
        }



    }else    //if (currentToolTippedQuad->quadLogicMode == 0) 
    { // is our quad a ground?!
        if (manageTooltipButton(Vec2(0, 0), Vec2(1, 1), "No options avaible", 1.0f, 1.0f, 1.0f)) {
            tooltipGuiStage = 0;
            currentToolTippedQuad = nullptr;
            return;
        }
    }

}


void engineLevel::menageIOFile() {

    if (M_getKeyboard()) {
        if (M_getKeyboard()->keyReleased(11)) isSaving = true;
        if (M_getKeyboard()->keyReleased(12)) isLoading = true;
    }

    if (!isSaving && !isLoading) return;

    if (pathEntered) {
        if (isSaving)   saveLevel();
        if (isLoading)  loadLevel();
    }
    else {
        if (isSaving)   saveLevelShowWindow();
        if (isLoading)  loadLevelShowWindow();
        //cout << "IS path entered: " << pathEntered << endl;
        if (!pathEntered) { isSaving = false; isLoading = false; }
    }
}
void engineLevel::saveLevelShowWindow() {

    pathEntered = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileSaveDialog* pFileSave;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileSave->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        /* CONVERSATION PWSTR TO STRING*/
                        size_t i;
                        char* pMBBuffer = (char*)malloc(FILEPATH_BUFFER_SIZE);
                        const wchar_t* pWCBuffer = pszFilePath;

                        // Conversion
                        wcstombs_s(&i, pMBBuffer, (size_t)FILEPATH_BUFFER_SIZE,
                            pWCBuffer, (size_t)FILEPATH_BUFFER_SIZE - 1); // -1 so the appended NULL doesn't fall outside the allocated buffer

                        // Free multibyte character buffer
                        if (pMBBuffer)
                        {
                            savePath = pMBBuffer;
                            pathEntered = true;
                            free(pMBBuffer);
                        }
                        ///MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileSave->Release();
        }
        CoUninitialize();
    }
};
void engineLevel::loadLevelShowWindow() {

    pathEntered = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        /* CONVERSATION PWSTR TO STRING*/
                        size_t i;
                        char* pMBBuffer = (char*)malloc(FILEPATH_BUFFER_SIZE);
                        const wchar_t* pWCBuffer = pszFilePath;

                        // Conversion
                        wcstombs_s(&i, pMBBuffer, (size_t)FILEPATH_BUFFER_SIZE,
                            pWCBuffer, (size_t)FILEPATH_BUFFER_SIZE - 1); // -1 so the appended NULL doesn't fall outside the allocated buffer

                        // Free multibyte character buffer
                        if (pMBBuffer)
                        {
                            loadPath = pMBBuffer;
                            pathEntered = true;
                            free(pMBBuffer);
                        }
                        else
                            isLoading = false;
                        ///MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
};
void engineLevel::saveLevel() {

    ofstream mF(savePath);

    int mapElementsToSave = levelQuadVector.size();

    mF << "MAPELEMENTSCOUNT " << to_string(mapElementsToSave) << endl;
    for (int i = 0; i < levelQuadVector.size(); i++) {

        engineLevelQuad* mQ = (engineLevelQuad*)levelQuadVector[i];
        mF << "MAPELEMENT QUAD" << endl;
        mF << "UNIQUEID " << to_string(mQ->quadUniqueID) << endl;
        for (char j = 0; j < 4; j++) {
            mF << "VX"<< to_string(j) << " " << to_string(mQ->quadVertRealPositions[j].x) << endl;
            mF << "VY"<< to_string(j) << " " << to_string(mQ->quadVertRealPositions[j].y) << endl;
            mF << "TX"<< to_string(j) << " " << to_string(mQ->quadTexCoords[j].x) << endl;
            mF << "TY"<< to_string(j) << " " << to_string(mQ->quadTexCoords[j].y) << endl;
        }
        mF << "TEXTURENAME " << mQ->quadTextureName << endl;
        mF << "PARALLAX " << to_string(mQ->quadParralaxAmount) << endl;
        mF << "LOGIC " << to_string(mQ->quadLogicMode) << endl;
        mF << "MAPELEMENT FINISH" << endl;
    }
    mF.close();
    cout << "file saved" << endl;

    isSaving = false;
    pathEntered = false;
};
void engineLevel::loadLevel() {

    vector<string>fileLines;
    string myDataLine;
    ifstream MyReadFile(loadPath);
    while (getline(MyReadFile, myDataLine)) {
        if (myDataLine.compare("") != 0)
            fileLines.push_back(myDataLine);
    }
    MyReadFile.close();

    

    engineLevelQuad* newMapQuad = nullptr;
    for (unsigned int i = 0; i < fileLines.size(); i++) {

        string lineParameter, lineValue;

        string fL = fileLines[i];

        vector<string> internal; // vector with line parts
        stringstream ss(fL); // Splitting line into smaller parts...
        string tok;
        while (getline(ss, tok, ' ')) internal.push_back(tok); // splitting job...

        if (internal.size() == 2) {
            lineParameter = internal[0];
            lineValue = internal[1];

            cout << i << " - " << lineParameter << " - " << lineValue << endl;

            if (lineParameter.compare("MAPELEMENT") == 0) {
                if (lineValue.compare("QUAD") == 0 )
                    newMapQuad = new engineLevelQuad();
                if (lineValue.compare("FINISH") == 0) {

                    levelQuadVector.push_back(newMapQuad);
                    newMapQuad = nullptr;
                }
                    
            }
            else {
                if (newMapQuad != nullptr) {

                    if (lineParameter.compare("UNIQUEID") == 0) newMapQuad->quadUniqueID = stoi(lineValue);
                    for (char j = 0; j < 4; j++) {
                        if (lineParameter.compare("VX" + to_string(j)) == 0) newMapQuad->quadVertRealPositions[j].x = stof(lineValue);
                        if (lineParameter.compare("VY" + to_string(j)) == 0) newMapQuad->quadVertRealPositions[j].y = stof(lineValue);
                        if (lineParameter.compare("TX" + to_string(j)) == 0) newMapQuad->quadTexCoords[j].x = stof(lineValue);
                        if (lineParameter.compare("TY" + to_string(j)) == 0) newMapQuad->quadTexCoords[j].y = stof(lineValue);
                    }
                    if (lineParameter.compare("TEXTURENAME") == 0) { newMapQuad->quadTextureName = lineValue; /*levelTextureNamesVector.push_back(lineValue);*/ }
                    if (lineParameter.compare("PARALLAX") == 0) newMapQuad->quadParralaxAmount = stof(lineValue);
                    if (lineParameter.compare("LOGIC") == 0) newMapQuad->quadLogicMode = (char)max(0,min(255, stoi(lineValue)));
                }
            }
        }
    }

    // -- Ereasing multiple names in texture names ...
    /*vector<string>levelTexturesIndividualNames;
    if (levelTexturesNames.size() > 0) {



    }*/



    



    // Close the file
    cout << "file loaded" << endl;
    isLoading = false;
    pathEntered = false;
};

void engineLevel::fillTexturesData() {


    std::cout << "Aditional textures for map editor :" << std::endl;
    WIN32_FIND_DATA FindData;
    HANDLE hFind;
    hFind = FindFirstFile(L"textures\\*.*", &FindData); //fullSearchPath.c_str()
    if (hFind == INVALID_HANDLE_VALUE) std::cout << "Error searching directory\n";
    do
    {
        wstring tfn = (wstring)FindData.cFileName;
        string strTempFilename(tfn.begin(), tfn.end());
        if (strTempFilename.compare("..")!=0 && strTempFilename.compare(".")!=0 && strTempFilename.compare(".map")!=0 )
        {
            texturesInFolder.push_back(strTempFilename);

            //cout << "PATH: " << strTempFilename << endl;
            //if (!Get_Texture(tempfilename))
            //{
            //    new Textureunit(tempfilename, 256, 256, 128, 128, 1, 1);
            //}
        }
    } while (FindNextFile(hFind, &FindData) > 0);

    if (GetLastError() != ERROR_NO_MORE_FILES)
    {
        std::cout << "Something went wrong during searching\n";
    }


}

vector<engineLevelQuad*> engineLevel::getLevelGroundQuads() {
    return levelQuadOnlyGroundVectorOutput;
}