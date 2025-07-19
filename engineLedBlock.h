#pragma once
#ifndef ENGINE_LED_BLOCK_H
#define ENGINE_LED_BLOCK_H

#include "engine.h"

struct colorPixelRequireChangeRegistry { // For set changes in "color pixel registry"...
    float r, g, b;
    bool smoothToNextFrame;
    bool toDelete;
    unsigned int frame;
};


struct colorPixelRegistry{ // Main base of frames, per pixel...
    float r, g, b;
    bool smoothToNextFrame;
};

struct colorPixelOutput { // Output data...
    unsigned char r, g, b;
    void clear() {
        r = 0; g = 0; b = 0;
    }
};

class engineLedBlock : public module
{

//private:
public:


    unsigned int myID;
    unsigned int framesToSetInputFrontBuffer; // How many frames we must reserve? - Input - to module
    unsigned int framesToSetInputBackBuffer; // How many frames we must reserve? - Input - in module

    unsigned int framesSetedOutputFrontBuffer; // How many frames we must reserve? - Output - in module
    unsigned int framesSetedOutputBackBuffer; // How many frames we must reserve? - Output - outside module

    double frameToPrepareFrontBuffer; // What frame i must prepare for other modules? - To module
    double frameToPrepareBackBuffer; // What frame i must prepare for other modules? - In module
    

    
    vector<colorPixelRequireChangeRegistry>colorPixelRequiredChangesInput; // Intake data to module
    vector<colorPixelRequireChangeRegistry>colorPixelRequiredChangesOutput; // data currently in module - to read by it.
    

    vector<colorPixelRegistry>colorPixelRegistryData; // Current light frames data in module


    colorPixelOutput currentOutputingPixelFrontBuffer; // Color preparing in module - for output
    colorPixelOutput currentOutputingPixelBackBuffer; // Color currently outputed - for another modules

     

    engineLedBlock( unsigned int id );
    ~engineLedBlock();
    void load();
    void synchro();
    void work();
    void draw();

    unsigned char getR();
    unsigned char getG();
    unsigned char getB();

    void setRGBS(float r, float g, float b, bool s, unsigned int frame);
    void deleteFrame( unsigned int frame);

    void setFramesCount(int frames);

    unsigned int getFramesCount();

    void prepareFrameForReading(long double frame);

    void menageRequiredChanges();


};


#endif 