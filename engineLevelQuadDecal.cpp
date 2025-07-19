#include "engine.h"

#include "engineLevelQuadDecal.h"
#include "engineLevelQuad.h"
#include "engineLevel.h"

#include "engineRenderer.h"
#include "engineMouse.h"

#include <math.h>


engineLevelQuadDecal::engineLevelQuadDecal()
{
    M_getEngine()->registerModule(this, "Engine Level Quad Decal");

    drawBuffer = new engineRenderDrawBuffer(8);

    for (int i = 0 ; i<4 ; i++)
        decalRangePositions[i].Clear();
    positionOnLevel.Clear();
    scale.Clear();
    offset.Clear();

    offset.x = 0.5f;
    offset.y = 0.5f;
    scale.x = 512.0f;
    scale.y = 512.0f;
}

engineLevelQuadDecal::~engineLevelQuadDecal() {

}


void engineLevelQuadDecal::load() {
    glTexture = M_getRender()->get_texture("test1.png");
}

void engineLevelQuadDecal::synchro() {

    drawBuffer->swapBuffers();

    if (M_getRender())
        M_getRender()->add_module(this, -1000);
}

void engineLevelQuadDecal::work() {

    Vec2 MousePos = M_getMouse()->getPosition();

    angle += 1.0f * deltaTime*0.01f;

    decalRangePositions[0] = MousePos + Vec2(-1000.0f, -1000.0f);
    decalRangePositions[1] = MousePos + Vec2(+1000.0f, -1000.0f);
    decalRangePositions[2] = MousePos + Vec2(+1000.0f, +1000.0f);
    decalRangePositions[3] = MousePos + Vec2(-1000.0f, +1000.0f);

    //drawBuffer->drawQuadGeneric(decalRangePositions[0], decalRangePositions[1], decalRangePositions[2], decalRangePositions[3], Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1));

    if (!M_getLevel()) return;
    
    vector<engineLevelQuad*> groundQuads = M_getLevel()->getLevelGroundQuads();

    int quads = 0;

    for (int i = 0; i < groundQuads.size(); i++) {

        engineLevelQuad* quad = groundQuads[i];
        if (!quad->quadIsToDestroy) {

            Vec2 a, b, c, d;
            a = getTextureCoOrdinates(quad->quadVertRealPositions[0]);
            b = getTextureCoOrdinates(quad->quadVertRealPositions[1]);
            c = getTextureCoOrdinates(quad->quadVertRealPositions[2]);
            d = getTextureCoOrdinates(quad->quadVertRealPositions[3]);

            /*, getTextureCoOrdinates(quad->quadVertRealPositions[0])
              , getTextureCoOrdinates(quad->quadVertRealPositions[1])
              , getTextureCoOrdinates(quad->quadVertRealPositions[2])
              , getTextureCoOrdinates(quad->quadVertRealPositions[3])
              */

            if ( ( a.Length() <2.0f && a.Length() > -1.0f)
                || (b.Length() < 2.0f && b.Length() > -1.0f)
                || (c.Length() < 2.0f && c.Length() > -1.0f)
                || (d.Length() < 2.0f && d.Length() > -1.0f) )
            {
                quads++;
                drawBuffer->drawQuadGeneric(
                    quad->quadVertRealPositions[0]
                    , quad->quadVertRealPositions[1]
                    , quad->quadVertRealPositions[2]
                    , quad->quadVertRealPositions[3]
                    , a
                    , b
                    , c
                    , d
                );
            }
        }
        //cout << quads << endl;
    }

}

void engineLevelQuadDecal::draw() {
    drawBuffer->render(glTexture);
}

Vec2 engineLevelQuadDecal::getTextureCoOrdinates(Vec2 VecPos) {

    Vec2 ret;
    ret.x = ( (VecPos.x - positionOnLevel.x) / scale.x );
    ret.y = ( (VecPos.y - positionOnLevel.y) / scale.y );
    if (angle != 0.0f)
        ret = lengthdir(pointDistance(offset, ret), pointDirection(offset, ret) + angle);

    return ret;

}