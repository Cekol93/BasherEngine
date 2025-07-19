#include "engineLevelQuad.h"

#include "engine.h"
#include "engineRenderer.h"

#include "engineKeyboard.h"
#include "engineMouse.h"

#include <math.h>


engineLevelQuad::engineLevelQuad() {

    M_getEngine()->registerModule(this, "Engine Level Quad");


    quadIsInEditorMode = false;
    quadIsActive = false;
    quadLoadingFailed = false;

    quadTextureID = (GLuint)NULL;
    quadTextureName = (string)"NullTexture";

    // Positioning Crap...
    for (char i = 0; i <= 3; i++) {
        quadTexCoords[i] = Vec2(0.0f, 0.0f);            //4 texture positions
        quadVertRealPositions[i] = Vec2(0.0f, 0.0f);    //4 real (not modify by paralax) vertex points...
        quadVertVisiblePositions[i] = Vec2(0.0f, 0.0f); //4 visible (modyfied by parallax) vertex points...
    }
    Vec2 quadCenterPoint = Vec2(0.0f, 0.0f);
    Vec2 quadVisibleCenterPoint = Vec2(0.0f, 0.0f);
    quadLongestDistanceInside = 0.0; //Naj dystans od srodka do rogu

    //Quad logic / job
    quadUniqueID = 0;
    quadSortID = 0;
    quadLogicMode = (char)0;
    quadParralaxAmount = 0.0;
    isParralaxEnabled = true;
    physicsBody = nullptr;

    //Editor Crap...
    quadIsSelectedInEditor = false;
    quadSelectedPart = (char)0;

    quadIsToDestroy = false;

    /*
        PURE DRAWING SHIT!!
    */

    frontBufferV = new float[8]; // 4(quad)x2(point)x2(texture&verts) FOR MULTITHREADING
    frontBufferT = new float[8];
    frontBufferC = new float[16];
    backBufferV = new float[8];
    backBufferT = new float[8];
    backBufferC = new float[16];

    for (char i = 0; i < 8; i++) {
        frontBufferV[i] = 0.0f;
        frontBufferT[i] = 0.0f;
        backBufferV[i] = 0.0f;
        backBufferT[i] = 0.0f;
    }
    for (char i = 0; i < 16; i++) {
        frontBufferC[i] = 1.0f;
        backBufferC[i] = 1.0f;
    }

    editedBufferV = frontBufferV;
    editedBufferT = frontBufferT;
    editedBufferC = frontBufferC;

    drawedBufferV = backBufferV;
    drawedBufferT = backBufferT;
    drawedBufferC = backBufferC;
    /*
        END OF DRAWING SHIT :o
    */

}
engineLevelQuad::~engineLevelQuad() {
    delete[] frontBufferV;
    delete[] frontBufferT;
    delete[] frontBufferC;
    delete[] backBufferV;
    delete[] backBufferT;
    delete[] backBufferC;
}

void engineLevelQuad::load() {

}
void engineLevelQuad::synchro() {
    
    quadSortID = (double)quadUniqueID + quadParralaxAmount * 1000.0;

    quadIsActiveBackBuffer =  quadIsActive;

    if (!quadIsActive) return;
    swapDrawingBuffer();
    if (M_getRender()) 
        M_getRender()->add_module(this, (float)quadSortID*0.0001f );
}
void engineLevelQuad::work() {

    calculatePositions();
    fillDrawingBuffer();
}
void engineLevelQuad::draw() {

    if (quadTextureID == NULL) {
        manageTexture();
        return;
    }

    glBindTexture(GL_TEXTURE_2D, quadTextureID); // DRAWING  ON SCREEN...
    glVertexPointer(2, GL_FLOAT, 0, drawedBufferV);
    glTexCoordPointer(2, GL_FLOAT, 0, drawedBufferT);
    glColorPointer(4, GL_FLOAT, 0, drawedBufferC);
    glDrawArrays(GL_QUADS, 0, 4 );

}
void engineLevelQuad::manageTexture() {


        if (quadTextureName.compare("") != 0 && quadTextureID == NULL) {
            if (M_getRender())
                quadTextureID = M_getRender()->get_texture(quadTextureName);

            if (quadTextureID == NULL)
                quadLoadingFailed = true;
        }

}
void engineLevelQuad::fillDrawingBuffer() {
    if (!quadIsActive) return;

    editedBufferV[0] = quadVertVisiblePositions[0].x;   //Texturing
    editedBufferV[1] = quadVertVisiblePositions[0].y;
    editedBufferV[2] = quadVertVisiblePositions[1].x;
    editedBufferV[3] = quadVertVisiblePositions[1].y;
    editedBufferV[4] = quadVertVisiblePositions[2].x;
    editedBufferV[5] = quadVertVisiblePositions[2].y;
    editedBufferV[6] = quadVertVisiblePositions[3].x;
    editedBufferV[7] = quadVertVisiblePositions[3].y; // End of texturing


    editedBufferT[0] = quadTexCoords[0].x;
    editedBufferT[1] = quadTexCoords[0].y;
    editedBufferT[2] = quadTexCoords[1].x;
    editedBufferT[3] = quadTexCoords[1].y;
    editedBufferT[4] = quadTexCoords[2].x;
    editedBufferT[5] = quadTexCoords[2].y;
    editedBufferT[6] = quadTexCoords[3].x;
    editedBufferT[7] = quadTexCoords[3].y;
}
void engineLevelQuad::swapDrawingBuffer() {
    float* Temp;//,*TempT; // SWAP BUFFERS FOR MULTITHREADING
    //SWAPING VERTEX BUFFER
    Temp = editedBufferV;
    editedBufferV = drawedBufferV;
    drawedBufferV = Temp;
    //SWAPING TEXCORD BUFFER
    Temp = editedBufferT;
    editedBufferT = drawedBufferT;
    drawedBufferT = Temp;
    //SWAPPING COLOR BUFFER
    Temp = editedBufferC;
    editedBufferC = drawedBufferC;
    drawedBufferC = Temp;
}

void engineLevelQuad::quadEditSelectPart(Vec2 cursorPos) {

    quadSelectedPart = (char)4;
    Vec2 A, B;

    A.x = (quadVertRealPositions[0].x + quadVertRealPositions[1].x) * 0.5f;
    A.y = (quadVertRealPositions[0].y + quadVertRealPositions[1].y) * 0.5f;
    B.x = (quadVertRealPositions[1].x + quadVertRealPositions[2].x) * 0.5f;
    B.y = (quadVertRealPositions[1].y + quadVertRealPositions[2].y) * 0.5f;
    if (dirDiv(pointDirection(A, cursorPos ), pointDirection(A, B)) < 0) quadSelectedPart = (char)1;

    A.x = (quadVertRealPositions[1].x + quadVertRealPositions[2].x) * 0.5f;
    A.y = (quadVertRealPositions[1].y + quadVertRealPositions[2].y) * 0.5f;
    B.x = (quadVertRealPositions[2].x + quadVertRealPositions[3].x) * 0.5f;
    B.y = (quadVertRealPositions[2].y + quadVertRealPositions[3].y) * 0.5f;
    if (dirDiv(pointDirection(A, cursorPos ), pointDirection(A, B)) < 0) quadSelectedPart = (char)2;

    A.x = (quadVertRealPositions[2].x + quadVertRealPositions[3].x) * 0.5f;
    A.y = (quadVertRealPositions[2].y + quadVertRealPositions[3].y) * 0.5f;
    B.x = (quadVertRealPositions[3].x + quadVertRealPositions[0].x) * 0.5f;
    B.y = (quadVertRealPositions[3].y + quadVertRealPositions[0].y) * 0.5f;
    if (dirDiv(pointDirection(A, cursorPos ), pointDirection(A, B)) < 0) quadSelectedPart = (char)3;

    A.x = (quadVertRealPositions[3].x + quadVertRealPositions[0].x) * 0.5f;
    A.y = (quadVertRealPositions[3].y + quadVertRealPositions[0].y) * 0.5f;
    B.x = (quadVertRealPositions[0].x + quadVertRealPositions[1].x) * 0.5f;
    B.y = (quadVertRealPositions[0].y + quadVertRealPositions[1].y) * 0.5f;
    if (dirDiv(pointDirection(A, cursorPos ), pointDirection(A, B)) < 0) quadSelectedPart = (char)0;
}
void engineLevelQuad::quadEdit(Vec2 moveVector) {

    float a, b, c, d, e, f, g, h;
    quadDataLock.lock();
    a = quadVertRealPositions[0].x;
    b = quadVertRealPositions[0].y;
    c = quadVertRealPositions[1].x;
    d = quadVertRealPositions[1].y;
    e = quadVertRealPositions[2].x;
    f = quadVertRealPositions[2].y;
    g = quadVertRealPositions[3].x;
    h = quadVertRealPositions[3].y;
    quadDataLock.unlock();

    switch (quadSelectedPart)
    {
    case 0: a += moveVector.x; b += moveVector.y; break;
    case 1: c += moveVector.x; d += moveVector.y; break;
    case 2: e += moveVector.x; f += moveVector.y; break;
    case 3: g += moveVector.x; h += moveVector.y; break;
    case 4:
        a += moveVector.x; b += moveVector.y;
        c += moveVector.x; d += moveVector.y;
        e += moveVector.x; f += moveVector.y;
        g += moveVector.x; h += moveVector.y;
        break;
    }

    quadSetVertexPositions(a,b,c,d,e,f,g,h);


}
void engineLevelQuad::quadEditEnd() {
    quadSelectedPart = (char)4;
    quadIsSelectedInEditor = false;
    quadSwapToGrid();

}
void engineLevelQuad::quadSwapToGrid() {

    
    Vec2 tempPos[4];
    quadDataLock.lock();
    for (int a = 0; a < 4; a++)
    {
        tempPos[a] = quadVertRealPositions[a];
        float x = negative(tempPos[a].x) * (int)((fabs(tempPos[a].x) + 4) / 8) * 8;
        float y = negative(tempPos[a].y) * (int)((fabs(tempPos[a].y) + 4) / 8) * 8;
        tempPos[a] = Vec2(x, y);
    }
    quadDataLock.unlock();

    quadSetVertexPositions
    ( tempPos[0].x
    , tempPos[0].y 
    , tempPos[1].x 
    , tempPos[1].y 
    , tempPos[2].x 
    , tempPos[2].y 
    , tempPos[3].x
    , tempPos[3].y 
    );
}


void engineLevelQuad::quadSetVertexPositions(float a, float b, float c, float d, float e, float f, float g, float h) {
    if (
        (dirDiv(pointDirection(Vec2(a, b), Vec2(c, d)), pointDirection(Vec2(c, d), Vec2(e, f))) < 0) &&
        (dirDiv(pointDirection(Vec2(c, d), Vec2(e, f)), pointDirection(Vec2(e, f), Vec2(g, h))) < 0) &&
        (dirDiv(pointDirection(Vec2(e, f), Vec2(g, h)), pointDirection(Vec2(g, h), Vec2(a, b))) < 0) &&
        (dirDiv(pointDirection(Vec2(g, h), Vec2(a, b)), pointDirection(Vec2(a, b), Vec2(c, d))) < 0)
        )
    {
        if ((pointDistance(Vec2(a, b), Vec2(c, d)) < 4089) && (pointDistance(Vec2(c, d), Vec2(e, f)) < 4089) &&
            (pointDistance(Vec2(e, f), Vec2(g, h)) < 4089) && (pointDistance(Vec2(g, h), Vec2(a, b)) < 4089))
        {
            quadDataLock.lock();
            quadVertRealPositions[0].Set(a, b);
            quadVertRealPositions[1].Set(c, d);
            quadVertRealPositions[2].Set(e, f);
            quadVertRealPositions[3].Set(g, h);
            quadDataLock.unlock();
        
            managePhysicBody();
        
        }
    }
}
void engineLevelQuad::quadSetTexureCoords(float a, float b, float c, float d, float e, float f, float g, float h) {
    quadTexCoords[0].Set(a, b);
    quadTexCoords[1].Set(c, d);
    quadTexCoords[2].Set(e, f);
    quadTexCoords[3].Set(g, h);
}
void engineLevelQuad::calculatePositions() {

    quadDataLock.lock();
    for (char i = 0; i < 4; i++)
        quadVertVisiblePositions[i] = quadVertRealPositions[i];
    quadDataLock.unlock();

    //quadParralaxAmount = 0.1 + sin(quadUniqueID * 0.25 + M_getEngine()->engineElapsedTime * 0.002) * 0.2;

    if (M_getRender() && quadParralaxAmount != 0.0 && isParralaxEnabled) {
        for (char i = 0; i < 4; i++) {
            quadVertVisiblePositions[i].x -= (quadVertVisiblePositions[i].x - M_getRender()->get_position_x()) * quadParralaxAmount;
            quadVertVisiblePositions[i].y -= (quadVertVisiblePositions[i].y - M_getRender()->get_position_y()) * quadParralaxAmount;
        }
    }

    quadCenterPoint.x = (quadVertVisiblePositions[0].x + quadVertVisiblePositions[1].x + quadVertVisiblePositions[2].x + quadVertVisiblePositions[3].x) * 0.25f;
    quadCenterPoint.y = (quadVertVisiblePositions[0].y + quadVertVisiblePositions[1].y + quadVertVisiblePositions[2].y + quadVertVisiblePositions[3].y) * 0.25f;

    //cout << pointDistance(quadCenterPoint, Vec2(M_getRender()->get_position_x(), M_getRender()->get_position_y())) << endl;

    if (pointDistance(quadCenterPoint, Vec2(M_getRender()->get_position_x(), M_getRender()->get_position_y())) > 4096.0f)
        quadIsActive = false; else quadIsActive = true;

}
void engineLevelQuad::managePhysicBody() {
    if (physicsBody == nullptr) {
        physicsBody = new enginePhysicsObject();
    }

    if (physicsBody != nullptr) {
        physicsBody->physicsSetLogicMode(0);
        physicsBody->physicsSetVertexPositions(quadVertRealPositions[0], quadVertRealPositions[1], quadVertRealPositions[2], quadVertRealPositions[3]);
    }
}



// -- EDITOR THINGS...
bool engineLevelQuad::isPointInside(Vec2 point) {
    if (
        (dirDiv(pointDirection(quadVertRealPositions[0], quadVertRealPositions[1]), pointDirection(quadVertRealPositions[0], point)) < 0) &&
        (dirDiv(pointDirection(quadVertRealPositions[1], quadVertRealPositions[2]), pointDirection(quadVertRealPositions[1], point)) < 0) &&
        (dirDiv(pointDirection(quadVertRealPositions[2], quadVertRealPositions[3]), pointDirection(quadVertRealPositions[2], point)) < 0) &&
        (dirDiv(pointDirection(quadVertRealPositions[3], quadVertRealPositions[0]), pointDirection(quadVertRealPositions[3], point)) < 0)
        )    return 1;
    else return 0;
}

Vec2 engineLevelQuad::getNearrestPointInside(Vec2 targetPoint) {
    
    if (isPointInside(targetPoint)) return targetPoint;

    Vec2 closestPoint;

    Vec2 AB = vectorSubtraction(quadVertVisiblePositions[1], quadVertVisiblePositions[0]);
    Vec2 BC = vectorSubtraction(quadVertVisiblePositions[2], quadVertVisiblePositions[1]);
    Vec2 CD = vectorSubtraction(quadVertVisiblePositions[3], quadVertVisiblePositions[2]);
    Vec2 DA = vectorSubtraction(quadVertVisiblePositions[0], quadVertVisiblePositions[3]);

    Vec2 AP = vectorSubtraction(targetPoint, quadVertVisiblePositions[0]);
    Vec2 BP = vectorSubtraction(targetPoint, quadVertVisiblePositions[1]);
    Vec2 CP = vectorSubtraction(targetPoint, quadVertVisiblePositions[2]);
    Vec2 DP = vectorSubtraction(targetPoint, quadVertVisiblePositions[3]);

    Vec2 projAB = vectorProjection(AP, AB);
    Vec2 projBC = vectorProjection(BP, BC);
    Vec2 projCD = vectorProjection(CP, CD);
    Vec2 projDA = vectorProjection(DP, DA);

    // Wyznaczanie punktu na krawêdzi czworok¹ta, który jest najbli¿ej celu
    closestPoint = quadVertVisiblePositions[0] + projAB;
    double minDistance = magnitude(targetPoint - closestPoint);

    Vec2 testPoint = quadVertVisiblePositions[1] + projBC;
    double distance = magnitude(targetPoint - testPoint);
    if (distance < minDistance) {
        minDistance = distance;
        closestPoint = testPoint;
    }

    testPoint = quadVertVisiblePositions[2] + projCD;
    distance = magnitude(targetPoint - testPoint);
    if (distance < minDistance) {
        minDistance = distance;
        closestPoint = testPoint;
    }

    testPoint = quadVertVisiblePositions[3] + projDA;
    distance = magnitude(targetPoint - testPoint);
    if (distance < minDistance) {
        closestPoint = testPoint;
    }

    return closestPoint;
}