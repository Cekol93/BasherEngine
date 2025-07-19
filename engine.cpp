#include "engine.h"

#include "engineRenderer.h"
#include "engineRendererSimpleSprite.h"
//#include "engineRendererTextureMenager.h"

#include "enginePrefetcher.h"
#include "engineProfiler.h"
#include "engineDebugInfo.h"

#include "engineXInput.h"
#include "engineKeyboard.h"
#include "engineMouse.h"
#include "engineSound.h"

#include "engineActorController.h"
#include "enginePhysics.h"

#include "engineLevel.h"
#include "engineLevelQuadDecal.h"


#include "engineLedController.h"
#include "engineSerialController.h"
#include "engineColorPicker.h"
#include "engineIOFile.h"

#include "dummyLoad.h"

string drawInfoBoolStatus(bool s) {
    if (M_getDebugInfo())
        return M_getDebugInfo()->boolStatus(s);
    return "";
}


void threadwork(void* aArg) {
    threadController* con = (threadController*)aArg;
    if (con)
    {
        con->gotThread = true; // do i got a thread to operate with?
        while (con->wait) { std::this_thread::sleep_for(2000ms); }
        while (con->keepWork == true) {

            
            LARGE_INTEGER TWST;
            LARGE_INTEGER TWET;
            LARGE_INTEGER freq;

            QueryPerformanceFrequency(&freq);
            QueryPerformanceCounter(&TWST); // threadWaitStartTime
            QueryPerformanceCounter(&TWET); // threadWaitStartTime

            if (con->toCompute) {
                con->work();
                QueryPerformanceCounter(&TWET); // threadWaitEndTime
                con->toCompute = false;
            }
            
            long double waitTime = (TWET.QuadPart - TWST.QuadPart) * 1000.0f / freq.QuadPart;
            long double wait = con->engineTargetFrameTime - waitTime;

            if ( wait > 2.0 )
                std::this_thread::sleep_for(chrono::milliseconds(1));
            else {
                std::this_thread::sleep_for(chrono::milliseconds(0));
            }

        }
    }
}


mutex _tm;
long double getTime() {
    _tm.lock();
    LARGE_INTEGER t, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    long double _ret = t.QuadPart * 1000.0 / freq.QuadPart;
    _tm.unlock();
    return _ret;
}

module::module() {
    moduleId = 0;
    destroyTimer = 0;
    moduleToDestroy = false;                // Czy usunac obiekt? 1-tak / 0-nie
    intakeComputeMiliseconds = 0.000001; // how many ms take to compute shit?
    outtakeComputeMiliseconds = 0.000001; // how many ms take to compute shit?

    requestChangeCore = true;
    lastAllignCore = -1;

    debugMessage = "";

    deltaTime = 0.000001;
    drawDepth = 1000.0;
    moduleName = "unnamed";
}
void module::load() {}
void module::synchro(){}
void module::work() {}
void module::draw() {}
string module::getDebugMessage() { return debugMessageOutput; }

module::~module() {}
//-------------------------------------------------------------------------------------------
threadController::threadController() {
    ID = 0;
    gotThread = false; // do i got a thread to operate with?
    keepWork = true; // keep thread alive?
    toCompute = false; // can i compute thread work?
    wait = true; // wait for init!
    engineTargetFrameTime = 1.0;

    computeMiliseconds = 0.0f;  // how many ms take to compute shit?
    computeMilisecondsPresumed = 0.0f;  // ms for sorting modules!


    frontModules = new module * [10000000];
    backModules = new module * [10000000];
    intakeModules = frontModules;
    outtakeModules = backModules;
    intakeModulesCount = 0;
    outtakeModulesCount = 0;


}
void threadController::addModule(module* M) {
    intakeModules[intakeModulesCount] = M;
    intakeModulesCount++;
};
void threadController::switchModulesList() {

    module** Temp;// SWAP BUFFERS FOR MULTITHREADING
    Temp = intakeModules;
    intakeModules = outtakeModules;
    outtakeModules = Temp;
    //SWAPING NUMBERS
    outtakeModulesCount = intakeModulesCount;
    intakeModulesCount = 0;

};
void threadController::work() {

    //float thread_time_start = get_time();

    LARGE_INTEGER tTS, tTE, mTS, mTE, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&tTS);
    for (unsigned int i = 0; i < outtakeModulesCount; i++) // Kopiujemy nowo stworzone obiekty, do wlasciwego kontenera.
    {
        QueryPerformanceCounter(&mTS);
        module* M = (module*)outtakeModules[i];
        M->work();
        QueryPerformanceCounter(&mTE);
        M->intakeComputeMiliseconds = ((long double)(mTE.QuadPart - mTS.QuadPart) * (long double)1000.0 / (long double)freq.QuadPart);
    }

    QueryPerformanceCounter(&tTE);
    computeMiliseconds = (long double)(tTE.QuadPart - tTS.QuadPart) * (long double)1000.0 / (long double)freq.QuadPart;
}
threadController::~threadController() {
    gotThread = false; // do i got a thread to operate with?
    keepWork = true; // keep thread alive?
    toCompute = false; // can i compute thread work?
    wait = true; // wait for init!
    computeMiliseconds = 0.0f;  // how many ms take to compute shit?
    computeMilisecondsPresumed = 0.0f;  // ms for sorting modules!
    delete[] frontModules;
    delete[] backModules;
}




engine* localEngine = NULL;
engine* M_getEngine() { return localEngine; }
long double engine::deltaTime() { return engineDeltaTime; }
long double engine::elapsedTime() { return engineElapsedTime; }

engine::engine() {

    modulesIdCounter = 0;
    NewModules.clear(); // our 'just created' modules
    Modules.clear(); // existed modules
    toDestroyModules.clear();

    Threads.clear(); // threads
    Controllers.clear(); // thread controllers
    systemCores = 0; // how many cores/threads we got?
    engineWork = false; //can engine work, or stop?
    //ms_smooth = 0.25f; // magical thing

    oneSecondTimer = 0.0f; // time counter
    framesPerSecond = 0; // how many frames we got?
    engineFrames = 0; // frames counter
    engineDeltaTime = 0.0; // how much time frame compute?
    engineElapsedTime = 0.0; // time till start;


    engineTargetFrames = 420; //60;
    engineTargetFrameTime = (long double)1000.000000000 / (long double)engineTargetFrames;

    waitForThreadsTime = 1; // in MS

    T1 = { 0 };
    T2 = { 0 };
    freq = { 0 };
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&T1);
    QueryPerformanceCounter(&T2);

    msSmooth = 1.0f;
    systemGraphics = true;

    localEngine = this;
}
void engine::init(int argc, char** argv) {
    srand((unsigned int)time(NULL));
    //timeBeginPeriod(1);

    systemCores = max(thread::hardware_concurrency() - 1, (unsigned int)1);
    //isControllerWorking = new bool[systemCores];

    for (int i = 0; i < systemCores ; ++i) {
        //isControllerWorking[i] = false;

        threadController* con = new threadController();
        con->ID = i;
        thread* thr = new thread(threadwork, con);
        Controllers.push_back(con);
        Threads.push_back(thr);
    }

    
    engineWork = true;
    //LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&T1);
    QueryPerformanceCounter(&T2);

    systemGraphics = initFreeGlut(argc, argv);

    new engineRender();
    //new engineRendererSimpleSprite();

    //new engineRenderTextureMenager();

    new enginePrefetcher();
    new engineProfiler();
    new engineDebugInfo();

    new engineXInput();
    new engineKeyboard();
    new engineMouse();
    
    new engineSound();

    new enginePhysics();
    new engineActorController();

    new engineLevel();

    new engineLevelQuadDecal();

    //new engineLedController();
    //new engineSerialController();
    //new engineColorPicker();
    //new engineIOFIle();




    new engineRendererSimpleSprite("star", "test2.png", 32, 32, 16, 16, 1, 1);
    //--
    //for (unsigned int i = 0; i < 20; i++)
    //   new dummyLoad();


}
void engine::loop_job() {
    //unsigned int i = 0;
    unsigned int j = 0;

    /*
        STARTING ACTUAL LOOP
    */
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&T2);

    engineDeltaTime = (T2.QuadPart - T1.QuadPart) * 1000.0f / freq.QuadPart;

    QueryPerformanceCounter(&T1);
    engineElapsedTime += engineDeltaTime;

    //Copying new modules...
    for (unsigned int i = 0; i < NewModules.size(); i++) // Kopiujemy nowo stworzone obiekty, do wlasciwego kontenera.
    {
        module* M = (module*)NewModules[i];
        M->load();
        Modules.push_back(M);
    }
    NewModules.clear();
    // Single path operations...
    for (unsigned int i = 0; i < Modules.size(); i++)
    {
        module* M = (module*)Modules[i];

        M->deltaTime = engineDeltaTime;
        M->synchro();
        M->outtakeComputeMiliseconds = M->intakeComputeMiliseconds;
        // Debug information...
        M->debugMessageOutput = M->debugMessage;
        M->debugMessage.clear();
        if (M->moduleToDestroy)
        {
            M->destroyTimer = 0;
            toDestroyModules.push_back(M);
            Modules.erase(Modules.begin() + i);
        }

    }
    // Modules to delete menagement...
    for (unsigned int i = 0; i < toDestroyModules.size(); i++)
    {
        module* M = (module*)toDestroyModules[i];
        M->destroyTimer++;
        if (M->destroyTimer>1024)
        {
            toDestroyModules.erase (toDestroyModules.begin()+i);
            delete M;
        }
    }


    for (unsigned int i = 0; i < Controllers.size(); i++) {
        Controllers[i]->switchModulesList();

        if (!Controllers[i]->keepWork)
            engineWork = false;
    }

    if (!engineWork) {
        if (M_getRender()) glutLeaveMainLoop();
        return;
    }

    /*if (M_getRender())
        M_getRender()->swapData();
    */

    if (M_getRender())
        M_getRender()->swapData();
    
    if (M_getPrefetcher()) {

        for (unsigned int i = 0; i < Controllers.size(); i++)
            M_getPrefetcher()->addController((threadController*)Controllers[i]);

        for (unsigned int i = 0; i < Modules.size(); i++)
            M_getPrefetcher()->addModule((module*)Modules[i]);

        if (M_getPrefetcher()->initialization) {
            M_getPrefetcher()->compute();

            for (unsigned int i = 0; i < Controllers.size(); i++) {
                Controllers[i]->engineTargetFrameTime = this->engineTargetFrameTime;
                Controllers[i]->wait = false;
            }
        }
    }

    for (unsigned int i = 0; i < Controllers.size(); i++) {
        threadController* tc = (threadController*)Controllers[i];
        tc->toCompute = true;
    }
    
    //std::this_thread::sleep_for(chrono::milliseconds(1));

    if (M_getRender()) { M_getRender()->draw(); }
    

    //engineTargetFrameTime


    LARGE_INTEGER TC;
    long double frameTime = 1.0;
    bool engineWait;

    do {
        engineWait = false;
        for (unsigned int i = 0; i < Controllers.size(); i++) {
            threadController* tc = (threadController*)Controllers[i];
            engineWait = max(engineWait, tc->toCompute);

            QueryPerformanceFrequency(&freq);
            QueryPerformanceCounter(&TC);
            frameTime = (TC.QuadPart - T1.QuadPart) * 1000.0f / freq.QuadPart;
            //cout << frameTime <<"|" << engineTargetFrameTime  << endl;
        }

        if (frameTime < engineTargetFrameTime) {
            long double wait = engineTargetFrameTime - frameTime;
            //std::cout << wait << endl;
            //cout << wait << endl;
            if (wait > 2.0) {
                //std::this_thread::sleep_for( chrono::milliseconds((int)ceill(wait))) ;
                std::this_thread::sleep_for( chrono::milliseconds(1));
                
            }
            else
                std::this_thread::sleep_for(chrono::milliseconds(0));

            if (wait > 0.0)
                engineWait = true;

        }

        /*if (engineWait) {
                std::this_thread::sleep_for(chrono::milliseconds(0)); 
        }*/

    }while (engineWait);

    




    /*bool engineWait = true;
    while (engineWait) {
        std::this_thread::sleep_for(chrono::milliseconds(1));
        //cout << "[";
        bool ce = false;
        for (unsigned int i = 0; i < Controllers.size(); i++) {
            threadController* tc = (threadController*)Controllers[i];
            ce = max(ce, tc->toCompute);
        //    cout << Controllers[i]->toCompute << ",";
        }
        engineWait = ce;
    }*/


    engineFrames++;
    oneSecondTimer += engineDeltaTime;
    if (oneSecondTimer >= 1000.0) {
        oneSecondTimer -= 1000.0;
        framesPerSecond = engineFrames;
        engineFrames = 0;

        //system("CLS");
        //cout << endl;
        //cout << "Engine FPS: " << framesPerSecond << endl;
        /*cout << "Controllers:" << this->Controllers.size() << " / Modules: " << this->Modules.size() << endl;
        for (unsigned int j = 0; j < Controllers.size(); j++) {
            threadController* c = Controllers[j];
            cout << "[" << c->ID << "] - :" << c->outtakeModulesCount <<" [" <<c->computeMiliseconds<<"]" << endl;
        }
        if (M_getSerialController()) {
            if (M_getSerialController()->isConnected())
            cout << "Data from serial port: " << M_getSerialController()->getBufferedData().c_str() << endl;
        }*/

    }
}
void engine::loop() {
   
    if (systemGraphics)
        glutMainLoop();
    else
    while (engineWork) {
        loop_job();
    }


    cout << "Stopping threads...";
    for (unsigned int i = 0; i < Controllers.size(); i++) {
        Controllers[i]->toCompute = false;
        Controllers[i]->keepWork = false;
    }
    cout << " done!" << endl << "Destroying threads...";

    for (unsigned int i = 0; i < Threads.size(); i++) // Ogarniamy obiekty, ich role, oraz niszczymy je (kiedy jest potrzeba)
    {
        thread* T = (thread*)Threads[i];
        T->join();
        delete T;
    }
    Threads.clear(); // threads
    cout << " done!" << endl << "Destroying controllers...";

    for (unsigned int i = 0; i < Controllers.size(); i++)
    {
        threadController* C = (threadController*)Controllers[i];
        delete C;
    }
    Controllers.clear();
    cout << " done!" << endl << "Destroying modules...";

    for (unsigned int i = 0; i < NewModules.size(); i++) // Ogarniamy obiekty, ich role, oraz niszczymy je (kiedy jest potrzeba)
    {
        module* M = (module*)NewModules[i];
        delete M;
    }
    NewModules.clear(); // our 'just created' modules
    cout << " ... ";

    for (unsigned int i = 0; i < Modules.size(); i++) // Ogarniamy obiekty, ich role, oraz niszczymy je (kiedy jest potrzeba)
    {
        module* M = (module*)Modules[i];
        delete M;
    }
    Modules.clear(); // existed modules
    cout << " done!" << endl;
    return;
}
void engineLoopFreeGlut() {
    if (M_getEngine())
        M_getEngine()->loop_job();
}
void engine::registerModule(module* NM, std::string Name) { 
    mLock.lock();
    NM->moduleId = modulesIdCounter;
    modulesIdCounter++;
    NewModules.push_back(NM); 
    NM->moduleName = Name;
    mLock.unlock();
}

void engine::mutexLock() {
    mLock.lock();
}
void engine::mutexUnlock() {
    mLock.unlock();
}
void engine::destroy() {
    cout << "End!" << endl;
    return;
}
void engine::stop() {
    engineWork = false;
}
engine::~engine() {}

bool engine::initFreeGlut(int argc, char** argv) {
    glutInit(&argc, argv);
    /*glutInitContextVersion(4, 0);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);*/

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_STENCIL);

    glutInitWindowSize(1280, 720);
    glutCreateWindow("");
    //glutFullScreen();

    glutIdleFunc(engineLoopFreeGlut);
    glutDisplayFunc(engineLoopFreeGlut);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    cout << "Graphics Module - ";
    GLenum GlewINIT = glewInit();
    if (GlewINIT != GLEW_OK)
    {
        cout << "Cannot initialize GLEW! Error: " << glewGetErrorString(GlewINIT) << endl;
        return false;
    }
    else
    {
        cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

        if (glewGetExtension("GL_ARB_fragment_program")) cout << " - GL_ARB_fragment_program enabled" << endl;

        typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int); // VSYNC!
        PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

        const char* extensions = (char*)glGetString(GL_EXTENSIONS);
        if (strstr(extensions, "WGL_EXT_swap_control") == 0)
            cout << " - Cant turn off Vsync!" << endl;
        else
        {
            wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
            if (wglSwapIntervalEXT) {
                wglSwapIntervalEXT(0);
                cout << " - Turning off Vsync..." << endl;
            }
        }

        return true;
    }
}


