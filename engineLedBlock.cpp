#include "engineLedBlock.h"
#include "engine.h"
#include <math.h>

#define M_PI 3.14159265358979323846

engineLedBlock::engineLedBlock(unsigned int id)
{
    M_getEngine()->registerModule( this , "engine LED Block");

    myID = id;


    framesToSetInputFrontBuffer = 1;
    framesToSetInputBackBuffer = 1;

    framesSetedOutputFrontBuffer = 1;
    framesSetedOutputBackBuffer = 1;

    colorPixelRegistryData.clear();

    currentOutputingPixelFrontBuffer.clear();
    currentOutputingPixelBackBuffer.clear();

    colorPixelRequiredChangesInput.clear();
    colorPixelRequiredChangesOutput.clear(); 

    colorPixelRegistryData.push_back({ 0.0f,0.0f,0.0f });
    colorPixelRegistryData.reserve(1024);
}

engineLedBlock::~engineLedBlock() {

}


void engineLedBlock::load() {

}

void engineLedBlock::synchro() {

    // Transporting all data from front buffer to back buffer...
    colorPixelRequiredChangesOutput = colorPixelRequiredChangesInput;
    colorPixelRequiredChangesInput.clear();
    
    framesToSetInputBackBuffer = framesToSetInputFrontBuffer;
    framesSetedOutputBackBuffer = framesSetedOutputFrontBuffer;

    frameToPrepareBackBuffer = frameToPrepareFrontBuffer;
    currentOutputingPixelBackBuffer = currentOutputingPixelFrontBuffer;
}

void engineLedBlock::work() {

    
    menageRequiredChanges();

    unsigned int totalFrames = (double)max((unsigned int)1 , (unsigned int)colorPixelRegistryData.size()-1 );
    unsigned int currentFrame = (unsigned int)floor( max(0.0,min((double)totalFrames-1,frameToPrepareBackBuffer) ) );
    unsigned int nextFrame = (currentFrame + 1) % totalFrames; //(unsigned int)floor(max(0.0, frameToPrepareBackBuffer));

    if (totalFrames == 1)
        nextFrame = 0;

    double fade = min(1.0,max(0.0,frameToPrepareBackBuffer - (double)currentFrame));

    double blendedR = colorPixelRegistryData[currentFrame].r + (colorPixelRegistryData[nextFrame].r- colorPixelRegistryData[currentFrame].r)* fade;
    double blendedG = colorPixelRegistryData[currentFrame].g + (colorPixelRegistryData[nextFrame].g- colorPixelRegistryData[currentFrame].g)* fade;
    double blendedB = colorPixelRegistryData[currentFrame].b + (colorPixelRegistryData[nextFrame].b- colorPixelRegistryData[currentFrame].b)* fade;

    currentOutputingPixelFrontBuffer.r = 1 + min((unsigned char)254, max((unsigned char)0, (unsigned char)blendedR)); //r
    currentOutputingPixelFrontBuffer.g = 1 + min((unsigned char)254, max((unsigned char)0, (unsigned char)blendedG)); //g
    currentOutputingPixelFrontBuffer.b = 1 + min((unsigned char)254, max((unsigned char)0, (unsigned char)blendedB)); //b

    //cout << "frameToPrepareBackBuffer:" << frameToPrepareBackBuffer << " currentFrame:" << currentFrame << " nextFrame:" << nextFrame << endl;
                                                               
}

void engineLedBlock::draw() {

}


unsigned char engineLedBlock::getR() {
    return currentOutputingPixelBackBuffer.r;
}
unsigned char engineLedBlock::getG() {
    return currentOutputingPixelBackBuffer.g;
}
unsigned char engineLedBlock::getB() {
    return currentOutputingPixelBackBuffer.b;
}
/*
void engineLedBlock::setR(float r, int frame) {
    colorPixelRegistry cpr = (colorPixelRegistry)colorPixelRegistryInput[frame];
    cpr.r = r;
    colorPixelRegistryInput[frame] = cpr;
}
void engineLedBlock::setG(float g, int frame) {
    colorPixelRegistry cpr = (colorPixelRegistry)colorPixelRegistryInput[frame];
    cpr.g = g;
    colorPixelRegistryInput[frame] = cpr;
}
void engineLedBlock::setB(float b, int frame) {
    colorPixelRegistry cpr = (colorPixelRegistry)colorPixelRegistryInput[frame];
    cpr.b = b;
    colorPixelRegistryInput[frame] = cpr;
}
void engineLedBlock::setS(bool s, int frame) {
    colorPixelRegistry cpr = (colorPixelRegistry)colorPixelRegistryInput[frame];
    cpr.smoothToNextFrame = s;
    colorPixelRegistryInput[frame] = cpr;
}
*/

void engineLedBlock::setRGBS(float r, float g, float b, bool s, unsigned int frame) {
    
    colorPixelRequireChangeRegistry reg;
    reg.r = r;
    reg.g = g;
    reg.b = b;
    reg.smoothToNextFrame = s;
    reg.frame = frame;
    reg.toDelete = false;

    colorPixelRequiredChangesInput.push_back(reg);
}

void engineLedBlock::deleteFrame( unsigned int frame ) {

    colorPixelRequireChangeRegistry reg;
    reg.r = 0.0f;
    reg.g = 0.0f;
    reg.b = 0.0f;
    reg.smoothToNextFrame = false;
    reg.frame = frame;
    reg.toDelete = true;
    colorPixelRequiredChangesInput.push_back(reg);
}

void engineLedBlock::setFramesCount(int frames) {
    framesToSetInputFrontBuffer = frames;
}


void engineLedBlock::menageRequiredChanges() {

    // Resizing current array from different vars...
    unsigned int highestFrameCount = 0;

    highestFrameCount = max(highestFrameCount, framesToSetInputBackBuffer);

    for (unsigned int i = 0; i < colorPixelRequiredChangesOutput.size(); i++) { // Get highest frame from change requests...
        colorPixelRequireChangeRegistry reg = colorPixelRequiredChangesOutput[i];
        highestFrameCount = max(highestFrameCount , reg.frame);
    }

    unsigned int privSize = colorPixelRegistryData.size();
    while ( highestFrameCount > colorPixelRegistryData.size() - 1) {

        colorPixelRegistry newPixelRegistry;
        newPixelRegistry.r = 0.0f;
        newPixelRegistry.g = 0.0f;
        newPixelRegistry.b = 0.0f;
        newPixelRegistry.smoothToNextFrame = false;
        colorPixelRegistryData.push_back(newPixelRegistry);
        cout << this << " resize! from:" << privSize << " To: " << colorPixelRegistryData.size() << endl;
    }
    // End of resize!
    
    //Filling array with data
    for (unsigned int i = 0; i < colorPixelRequiredChangesOutput.size(); i++) {

        colorPixelRequireChangeRegistry reg = colorPixelRequiredChangesOutput[i];
        unsigned int frame = reg.frame;
        colorPixelRegistryData[frame].r = reg.r;
        colorPixelRegistryData[frame].g = reg.g;
        colorPixelRegistryData[frame].b = reg.b;
        colorPixelRegistryData[frame].smoothToNextFrame = reg.smoothToNextFrame;
    }

    //Removing data
    for (unsigned int i = 0; i < colorPixelRequiredChangesOutput.size(); i++) {

        colorPixelRequireChangeRegistry reg = colorPixelRequiredChangesOutput[i];
        unsigned int frame = reg.frame;
        bool toDelete = reg.toDelete;
        if (toDelete == true) {

            cout << " Deleting! " << colorPixelRegistryData.size() <<" / "<< frame << endl;
            colorPixelRegistryData.erase(colorPixelRegistryData.begin() + frame );
            cout << " Shoud be deleted! " << colorPixelRegistryData.size() << endl;
        }
    }

    colorPixelRequiredChangesOutput.clear();
    framesSetedOutputFrontBuffer = colorPixelRegistryData.size();    
}

unsigned int engineLedBlock::getFramesCount() {
    return framesSetedOutputBackBuffer;
}

void engineLedBlock::prepareFrameForReading(long double frame) {
    frameToPrepareFrontBuffer = frame;
}
