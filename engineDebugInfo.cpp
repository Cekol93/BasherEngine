#include "engine.h"
#include "engineMath.h"
#include "engineDebugInfo.h"
#include "engineKeyboard.h"
#include "engineRenderer.h"
#include "engineRendererDrawBuffer.h"

engineDebugInfo* localDebugInfo = NULL;
engineDebugInfo* M_getDebugInfo() { return localDebugInfo; }

void engineDebugInfo::load()
{
    GlTexture = M_getRender()->get_texture("menu_font.png");
    //GL_Bac_Texture = M_getRender()->get_texture("menu_font.png");
}

engineDebugInfo::engineDebugInfo()
{
    localDebugInfo = this;
    chars = new engineRenderDrawBuffer(4096);

    inputDebugInfo.clear();
    outputDebugInfo.clear();

    GlTexture = -1;
    visible = false;
    active = false;

    M_getEngine()->registerModule(this, "Engine Debug Info");
}

engineDebugInfo::~engineDebugInfo()
{

}

void engineDebugInfo::synchro() {

    outputDebugInfo = inputDebugInfo;
    inputDebugInfo.clear();

    if (!visible) return;
    if (M_getRender()) M_getRender()->add_module(this, -9999);
    chars->swapBuffers();
    
    
}
string engineDebugInfo::boolStatus(bool s) {
    if (s) return (string)"#6YES#0"; else return (string)"#3NO#0";
}
void engineDebugInfo::drawText(Vec2 pos, Vec2 scale, string text) {

    //char editChar;
    bool edit = false;
    Vec2 inputPos = pos;
    Vec2 cursorPos = pos; //Vec2(pos.x - (+64 + str.length()*64)*scale.x*0.5f ,pos.y);
    float _eff = sin( (float)M_getEngine()->elapsedTime()*0.01f  ) * 0.4f;
    //cout << M_getEngine()->elapsedTime() << " / " << _eff << endl;
    for (std::string::iterator c = text.begin(); c != text.end(); ++c) {

        char currChar = max(0, min((int)*c, 255));
        if (currChar == 35) edit = true; // "^" char
        else
        {
            if (edit == true) {
                edit = false;
                
                switch (currChar)
                {
                case 'e': // End of module information
                    cursorPos.x = inputPos.x;
                    cursorPos.y += 100 * scale.y;
                    break;
                case 'n': // new line
                    cursorPos.x = inputPos.x;
                    cursorPos.y += 55 * scale.y;
                    break;

                case '0': // White color
                    chars->setColor(1.0f, 1.0f, 1.0f);
                    break;
                case '1': // Gray color
                    chars->setColor(0.6f, 0.6f, 0.6f);
                    break;
                case '2': // Black color
                    chars->setColor(0.3f, 0.3f, 0.3f);
                    break;
                case '3': // Red color
                    chars->setColor(0.6f+_eff, 0.1f, 0.1f);
                    break;
                case '4': // Orange color
                    chars->setColor(1.0f, 0.5f, 0.1f);
                    break;
                case '5': // Yellow color
                    chars->setColor(1.0f, 1.0f, 0.1f);
                    break;
                case '6': // Green color
                    chars->setColor(0.5f, 1.0f, 0.1f);
                    break;
                case '7': // Fusha color
                    chars->setColor(0.1f, 1.0f, 0.5f);
                    break;
                case '8': // WhiteBlue color
                    chars->setColor(0.1f, 1.0f, 1.0f);
                    break;
                case '9': // Blue color
                    chars->setColor(0.1f, 0.1f, 1.0f);
                    break;
                default: {}
                }
            }
            else {
                cursorPos.x += 55 * scale.x;
                chars->drawTile(currChar, cursorPos, Vec2(64 * scale.x, 64 * scale.y), 0, Vec2(0.5, 0.5), 16, 16);
            }
        }
    }

}

void engineDebugInfo::work() {


    if (M_getKeyboard()) // TOGGLE MENU
        if (M_getKeyboard()->keyPressed(2)) visible = !visible;

    if (!visible) {
        return;
    }

    // Smoothing information...

    Vec2 DP = Vec2(-1425, -1300);//Vec2( -M_getRender()->get_windowsize_w()*(1/M_getRender()->get_scale_w())/2  ,-M_getRender()->get_windowsize_h()*(1/M_getRender()->get_scale_h())/2 );
    
    chars->setColor(0.75f, 1.0f, 0.75f);
    drawText(DP, Vec2(1.5f, 1.5f), "Engine debug info #0:#1:#2:#3:#4:#5:#6:#7:#8:#9:");
    chars->setColor(1.0f, 1.0f, 1.0f);

    DP.x += 50;    DP.y += 100;

    drawText(DP, Vec2(0.75f, 0.75f), outputDebugInfo ); DP.y += 100;
    outputDebugInfo.clear();
}

void engineDebugInfo::draw() {

    if (GlTexture < 0) return;
    glPushMatrix();
    glTranslated(M_getRender()->get_position_x(), M_getRender()->get_position_y(), 0);
    chars->render(GlTexture);
    glPopMatrix(); // END DRAWING SCENE
}

void engineDebugInfo::addDebugMessage(string msg) {
    m.lock();
        inputDebugInfo.append(msg);
    m.unlock();
}

