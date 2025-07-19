#include "engine.h"
#include "engineProfiler.h"
#include "engineKeyboard.h"
#include "engineRenderer.h"
#include "engineRendererDrawBuffer.h"

engineProfiler* localProfiler = NULL;
engineProfiler* M_getProfiler() { return localProfiler; }

//void profiler::add_module(module* M , float executeTime ) {  moduleInfo Que = {M,executeTime}; Modules_info.push_back(Que);  }
void engineProfiler::addModule(module* M, long double executeTime) {
    moduleInfo Que = { M,executeTime };
    intakeModules[intakeModulesCount] = Que;
    intakeModulesCount++;
}
void engineProfiler::addCore(int number, long double executeTime, int modules) {
    coreInfo Que = { number,executeTime,modules };
    intakeCores[intakeCoresCount] = Que;
    intakeCoresCount++;
}

void engineProfiler::load()
{
    GlTexture = M_getRender()->get_texture("menu_font.png");
    //GL_Bac_Texture = M_getRender()->get_texture("menu_font.png");
}

engineProfiler::engineProfiler()
{
    localProfiler = this;
    M_getEngine()->registerModule(this, "Engine Profiler");

    chars = new engineRenderDrawBuffer(4096);

    intakeModulesCount = 0;
    outtakeModulesCount = 0;
    intakeCoresCount = 0;
    outtakeCoresCount = 0;
    outtakeCoresCountSmooth = 0;

    //ARRAYS:
    frontModules = new moduleInfo[128];// array
    backModules = new moduleInfo[128];// array

    frontCores = new coreInfo[512];// array
    backCores = new coreInfo[512]; // array
    backCoresSmooth = new coreInfo[512]; // array

    //POINTERS:
    intakeModules = frontModules; // pointer
    intakeCores = frontCores; // pointer

    outtakeModules = backModules; // pointer
    outtakeCores = backCores; // pointer


    drawFrames = 0;
    countedFrames = 0;
    oneSecTimer = 0;


    renderDtSmooth = 0.00;
    engineDtSmooth = 0.0f;
    smoothFactor = 0.001f;


    GlTexture = -1;
    visible = false;
    active = false;

   
}

engineProfiler::~engineProfiler()
{

}

void engineProfiler::synchro() {

    if (!visible) return;
    if (M_getRender()) M_getRender()->add_module(this, -9999);
    chars->swapBuffers();


    outtakeModulesCount = intakeModulesCount;
    outtakeCoresCount = intakeCoresCount;
    intakeCoresCount = 0;
    intakeModulesCount = 0;

    //POINTERS:
    moduleInfo* mI = outtakeModules;
    outtakeModules = intakeModules;
    intakeModules = mI;

    coreInfo* cI = outtakeCores;
    outtakeCores = intakeCores;
    intakeCores = cI;

}

void engineProfiler::drawText(Vec2 pos, Vec2 scale, string text) {

    Vec2 Pos = pos; //Vec2(pos.x - (+64 + str.length()*64)*scale.x*0.5f ,pos.y);
    for (std::string::iterator c = text.begin(); c != text.end(); ++c) {
        Pos.x += 55 * scale.x;
        chars->drawTile(max(0, min((int)*c, 255)), Pos, Vec2(64 * scale.x, 64 * scale.y), 0, Vec2(0.5, 0.5), 16, 16);
    }

}

void engineProfiler::work() {


    if (M_getKeyboard()) // TOGGLE MENU
        if (M_getKeyboard()->keyPressed(1)) visible = !visible;

    //active = 1;
    if (!visible) {
        intakeCoresCount = 0;
        outtakeCoresCount = 0;

        intakeModulesCount = 0;
        outtakeModulesCount = 0;
        return;
    }

    // Smoothing information...
    renderDtSmooth += (M_getRender()->deltaTime() - renderDtSmooth) * min((long double)1.00, smoothFactor * (deltaTime));
    engineDtSmooth += (M_getEngine()->deltaTime() - engineDtSmooth) * min((long double)1.00, smoothFactor * (deltaTime));

    if (outtakeCoresCountSmooth == 0 || (outtakeCoresCountSmooth != outtakeCoresCount) )
    {
        outtakeCoresCountSmooth = 0;
        for (int a = 0; a < outtakeCoresCount; a++) {
            coreInfo CI = (coreInfo)outtakeCores[a];
            backCoresSmooth[outtakeCoresCountSmooth] = CI;
            outtakeCoresCountSmooth++;
        }
    }
    for (int a = 0; a < outtakeCoresCount; a++) {
        coreInfo CI = (coreInfo)outtakeCores[a];

        for (int b = 0; b < outtakeCoresCountSmooth; b++) {
            coreInfo* CIS = &backCoresSmooth[b];

            if (CI.number == CIS->number) {
                CIS->executeTime += (CI.executeTime - CIS->executeTime) * min((long double)1.00, smoothFactor * (deltaTime));
                CIS->modules = CI.modules;
            }
        }
    }


    countedFrames += 1;
    oneSecTimer += this->deltaTime;
    if (oneSecTimer > 1000.0) { oneSecTimer -= 1000.0; drawFrames = countedFrames; countedFrames = 0; }

    Vec2 DP = Vec2(-1425, -1300);//Vec2( -M_getRender()->get_windowsize_w()*(1/M_getRender()->get_scale_w())/2  ,-M_getRender()->get_windowsize_h()*(1/M_getRender()->get_scale_h())/2 );


    chars->setColor(0.75f, 1.0f, 0.75f);
    drawText(DP, Vec2(1.5f, 1.5f), "Engine profiler"); DP.y += 100;
    chars->setColor(1.0f, 1.0f, 1.0f);
    drawText(DP + Vec2(100, 0), Vec2(1.0f, 1.0f), "Modules with highest execution time:"); DP.y += 80;

    for (int a = 0; a < outtakeModulesCount; a++) {
        moduleInfo MI = (moduleInfo)outtakeModules[a];

        string fstr; char buff[128];
        sprintf_s(buff, "%i. %s: %f.ms", a, MI.M->moduleName.c_str(), (float)MI.executeTime); fstr = buff;
        drawText(DP + Vec2(200, 0), Vec2(0.75f, 0.75f), fstr); DP.y += 70;
    }DP.y += 40;

    //outModulesInfo->clear();

    chars->setColor(1.0f, 1.0f, 0.25f);
    drawText(DP + Vec2(100, 0), Vec2(1.0f, 1.0f), "Threads timings:");
    drawText(DP + Vec2(1650, 0), Vec2(1.0f, 1.0f), "Ussage:");
    DP.y += 80;

    if (M_getRender()) {
        string fstr; char buff[128];
        chars->setColor(0.70f, 0.70f, 1.0f);

        sprintf_s(buff, "0. %.2f.ms Rendering ", (float)renderDtSmooth); fstr = buff;
        drawText(DP + Vec2(200, 0), Vec2(0.75f, 0.75f), fstr);
        //cout<<"Graphics"<<" "<<(float)M_getRender()->deltaTime()<<endl;
        //cout<<"deltaTime"<<" "<<(float)deltaTime*1000.0f<<endl;

        drawText(DP + Vec2(1500, 0), Vec2(0.75f, 0.75f), "[");
        float EF = renderDtSmooth / max((long double)1.0, engineDtSmooth);
        sprintf_s(buff, "] %i %%", (int)round(EF * 100)); fstr = buff;
        drawText(DP + Vec2(2500, 0), Vec2(0.75f, 0.75f), fstr);
        drawText(DP + Vec2(1510 + (2490 - 1510) * (EF), 0), Vec2(0.75f, 0.75f), "|");
        DP.y += 60;
    }

    //--
    float sc = min(0.75f, 24.0f / (float)outtakeCoresCountSmooth);
    for (int a = 0; a < outtakeCoresCountSmooth; a++) {


        coreInfo CI = (coreInfo)backCoresSmooth[a];

        float EF = min((long double)1.0, CI.executeTime / max((long double)1.0, engineDtSmooth));
        chars->setColor(1.0f, 1.0f - EF * 0.75f, 1.0f - EF * 0.75f);

        string fstr; char buff[128];
        sprintf_s(buff, "%i. %.2f.ms [%i]Modules", CI.number, (float)CI.executeTime, CI.modules); fstr = buff;
        drawText(DP + Vec2(200, 0), Vec2(0.75f, sc), fstr);

        drawText(DP + Vec2(1500, 0), Vec2(0.75f, sc), "[");

        sprintf_s(buff, "] %i %%", (int)round(EF * 100)); fstr = buff;
        drawText(DP + Vec2(2500, 0), Vec2(0.75f, sc), fstr);

        drawText(DP + Vec2(1510 + (2490 - 1510) * (EF), 0), Vec2(0.75f, sc), "|");

        DP.y += 60 * sc;

    }DP.y += 100;

    string fstr;
    char buff[128];
    sprintf_s(buff, "Engine FPS: %i / %f.ms", drawFrames, (float)M_getEngine()->deltaTime());
    fstr = buff;

    chars->setColor(0.75f, 0.75f, 0.75f);
    drawText(DP + Vec2(200, 0), Vec2(1.0f, 1.0f), fstr); DP.y += 70;

}

void engineProfiler::draw() {

    if (GlTexture < 0) return;
    glPushMatrix();
    glTranslated(M_getRender()->get_position_x(), M_getRender()->get_position_y(), 0);
    chars->render(GlTexture);
    glPopMatrix(); // END DRAWING SCENE
}

