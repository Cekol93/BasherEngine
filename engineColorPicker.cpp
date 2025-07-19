#include "engineColorPicker.h"
#include "engine.h"

#include "engineRenderer.h"
#include "engineRendererDrawBuffer.h"

#include "engineMouse.h"
#include "engineKeyboard.h"

#include <math.h>
#define M_PI 3.14159265358979323846

engineColorPicker* localColorPicker = NULL;
engineColorPicker* M_getColorPicker() { return localColorPicker; }

engineColorPicker::engineColorPicker()
{
    localColorPicker = this;
    M_getEngine()->registerModule(this, "Engine Color Picker");
    pickedRedFrontBuffer = pickedGreenFrontBuffer = pickedBlueFrontBuffer = 255.0;
    openedFrontBuffer = false;
    openedBackBuffer = false;

    drawBuffer = new engineRenderDrawBuffer(4096);
}

engineColorPicker::~engineColorPicker() {

}


void engineColorPicker::load() {
    glFontTexture = M_getRender()->get_texture("menu_font.png");
}

void engineColorPicker::synchro() {

    openedBackBuffer = openedFrontBuffer;

    if (openedFrontBuffer) {

        pickedRedBackBuffer = pickedRedFrontBuffer;
        pickedGreenBackBuffer = pickedGreenFrontBuffer;
        pickedBlueBackBuffer = pickedBlueFrontBuffer;

        if (M_getRender())
            M_getRender()->add_module(this, -1 );
        drawBuffer->swapBuffers();

    }
}

void engineColorPicker::work() {

    if (M_getKeyboard())
        openedFrontBuffer = M_getKeyboard()->key(67);

    if (!openedFrontBuffer) return;

    float PPI = 2 * M_PI / 3;

    double mouseRangeX = 2500;
    double mouseRangeY = 2500;


    for (int xx = -17; xx < 17; xx++)
        for (int yy = -17; yy < 17; yy++) {

            double ProcX =  min(1.0, max(0.0 , ((double)xx + 16.0) / 32.0 ));
            double ProcY =  min(1.0, max(0.0, ((double)yy + 16.0) / 32.0));

            long double ir = 0.5 + 0.5 * sin((ProcX*M_PI*2.0));
            long double ig = 0.5 + 0.5 * sin((ProcX*M_PI*2.0) + PPI);
            long double ib = 0.5 + 0.5 * sin((ProcX*M_PI*2.0) + PPI * 2);

            if (ProcY < 0.5) {
                long double w = (1.0 - (ProcY * 2));
                ir = max(w, ir);
                ig = max(w, ig);
                ib = max(w, ib);
            }

            if (ProcY > 0.5) {
                ir *= (1.0 - (ProcY - 0.5) * 2.0);
                ig *= (1.0 - (ProcY - 0.5) * 2.0);
                ib *= (1.0 - (ProcY - 0.5)*2.0 );
            }

            drawBuffer->setColor(ir,ig,ib);

            Vec2 Pos;
            Pos.Set(xx * 75 + 37, yy * 75 + 37);
            drawBuffer->drawTile(133, Pos, Vec2(90,90), 0, Vec2(0.5, 0.5), 16, 16);
        }

    if (M_getMouse()) {
        double mouseProcX = min(1.0, max(0.0, 0.5 + (double)M_getMouse()->getPosition().x / mouseRangeX ));
        double mouseProcY = min(1.0, max(0.0, 0.5 + (double)M_getMouse()->getPosition().y / mouseRangeY ));

        long double ir = 0.5 + 0.5 * sin((mouseProcX * M_PI * 2.0));
        long double ig = 0.5 + 0.5 * sin((mouseProcX * M_PI * 2.0) + PPI);
        long double ib = 0.5 + 0.5 * sin((mouseProcX * M_PI * 2.0) + PPI * 2);

        if (mouseProcY < 0.5) {
            long double w = (1.0 - (mouseProcY * 2));
            ir = max(w, ir);
            ig = max(w, ig);
            ib = max(w, ib);
        }

        if (mouseProcY > 0.5) {
            ir *= (1.0 - (mouseProcY - 0.5) * 2.0);
            ig *= (1.0 - (mouseProcY - 0.5) * 2.0);
            ib *= (1.0 - (mouseProcY - 0.5) * 2.0);
        }

        
        drawBuffer->setColor
        (ir
            , ig
            , ib
        );
        drawBuffer->drawTile(133, Vec2( (mouseProcX-0.5) * mouseRangeX, (mouseProcY-0.5)* mouseRangeY ), Vec2(256, 256), 0, Vec2(0.5, 0.5), 16, 16);
    
        pickedRedFrontBuffer =  ir * 255.0;
        pickedGreenFrontBuffer = ig * 255.0;
        pickedBlueFrontBuffer =  ib * 255.0;
    }

    drawBuffer->setColor(1.0f,1.0f,1.0f);
    drawText(Vec2(-1280.0f, 1380.0f), Vec2(1.0f, 1.0f), "Color Picker R:"+to_string(pickedRedFrontBuffer)+" G:" + to_string(pickedGreenFrontBuffer) + " B:" + to_string(pickedBlueFrontBuffer));


}

void engineColorPicker::drawText(Vec2 pos, Vec2 scale, string text) {
    //char editChar;
    bool edit = false;
    Vec2 inputPos = pos;
    Vec2 cursorPos = pos; //Vec2(pos.x - (+64 + str.length()*64)*scale.x*0.5f ,pos.y);
    float _eff = sin((float)M_getEngine()->elapsedTime() * 0.01f) * 0.4f;

    for (std::string::iterator c = text.begin(); c != text.end(); ++c) {
        char currChar = max(0, min((int)*c, 255));
        cursorPos.x += 55 * scale.x;
        drawBuffer->drawTile(currChar, cursorPos, Vec2(64 * scale.x, 64 * scale.y), 0, Vec2(0.5, 0.5), 16, 16);

    }
}

void engineColorPicker::draw() {

    if (glFontTexture > 0 && openedBackBuffer) {
        glPushMatrix();
        glTranslated(M_getRender()->get_position_x(), M_getRender()->get_position_y(), 0);
        drawBuffer->render(glFontTexture);
        glPopMatrix(); // END DRAWING SCENE
    }

}

bool engineColorPicker::isOpened() { return openedBackBuffer;  }
double engineColorPicker::getR() { return pickedRedBackBuffer; }
double engineColorPicker::getG() { return pickedGreenBackBuffer; }
double engineColorPicker::getB() { return pickedBlueBackBuffer; }