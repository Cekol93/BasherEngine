#pragma once
#ifndef ENGINE_H
#define ENGINE_H


#include <algorithm> // sort std
#include <string> // string std
#include <vector> // vector std
#include <time.h> // time

#include <chrono> // Mutexes
#include <thread> // Threading
#include <mutex>

#include <iostream> //Console Ii/Out

//#define GLEW_STATIC
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <windows.h> //OPENGL API

using namespace std;




long double getTime(); // get application worktime
//long double getElapsedTime(); // get application worktime

class module // ogolna klasa kazdego obiektu!
{
public:

    signed long int moduleId;
    bool moduleToDestroy;   //  Czy usunac obiekt? 1-tak / 0-nie
    unsigned int destroyTimer;


    long double intakeComputeMiliseconds; // how many ms take to compute shit?
    long double outtakeComputeMiliseconds; // how many ms take to compute shit?
    bool requestChangeCore;
    int lastAllignCore;


    string debugMessage;//String for debug messages, use #+char to aditional formating. See "engineDebugInfo" for more info.
    string debugMessageOutput;

    string moduleName;
    long double deltaTime;
    long double drawDepth;

    module();
    virtual ~module();
    virtual void load();
    virtual void synchro();
    virtual void work();
    virtual void draw();
    string getDebugMessage();


};

class threadController
{

public:
    bool gotThread; // do i got a thread to operate with?
    bool keepWork; // keep thread alive?
    bool toCompute; // can i compute thread work?
    bool wait; // wait for init!
    int ID;

    long double engineTargetFrameTime;
    
    long double computeMiliseconds; // how many ms take to compute shit?
    long double computeMilisecondsPresumed; // ms for sorting modules!


    int intakeModulesCount;
    int outtakeModulesCount;
    //ARRAYS:
    module** frontModules; // array
    module** backModules; // array
    //POINTERS:
    module** intakeModules; // pointer
    module** outtakeModules; // pointer

    void addModule(module*);

    void switchModulesList();
    void clearModulesList();

    threadController();
    ~threadController();
    void work();
};

class engine
{
public:
    engine();
    ~engine();
    void init(int, char**);
    bool initFreeGlut( int , char** );

    void destroy();
    void stop();
    void loop();
    void loop_job();
    void registerModule(module*, string);
    long double deltaTime();
    long double elapsedTime();

    void mutexLock();
    void mutexUnlock();

    mutex mLock;

    signed long int modulesIdCounter;
    vector < module* > NewModules; // our 'just created' modules
    vector < module* > Modules; // existed modules
    vector < module* > toDestroyModules; // or modules to destroy

    vector < thread* > Threads; // threads
    vector < threadController* > Controllers; // thread controllers

    unsigned int systemCores; // how many cores/threads we got?
    bool systemGraphics;

    //bool* isControllerWorking; // Array with synchronisation table , of controllers work

    bool engineWork; //can engine work, or stop?

    float msSmooth; // magical thing

    unsigned int engineFrames; // frames counter
    long double oneSecondTimer; // time counter
    long double framesPerSecond; // how many frames we got?
    long double engineDeltaTime; // how much time frame compute?
    long double engineElapsedTime;

    unsigned int engineTargetFrames;
    long double engineTargetFrameTime;

    int waitForThreadsTime; // how many ms we wait for checking out threads do they finish job

    LARGE_INTEGER T1, T2, freq;
};
engine* M_getEngine();

string drawInfoBoolStatus(bool s);
#endif // ENGINE_H
