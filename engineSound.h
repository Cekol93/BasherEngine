#pragma once
#ifndef ENGINE_SOUND_H
#define ENGINE_SOUND_H

#include "engine.h"

#include "al.h"
#include "alc.h"
#include "efx.h"
#include "efx-creative.h"
#include "EFX-Util.h"
#include "xram.h"

#include "engineSoundDevices.h"
#include "engineSoundCWaves.h"


#define MAXBUFERS 128

class engineSound : public module
{
public:
    engineSound();
    ~engineSound();
    void load();
    void synchro();
    void work();
    void draw();

    void initialize();


    void PropertieSmooth(ALuint uiEffect, ALenum pname, float Data);
    void PropertieAdvBlend(ALuint uiEffect, ALenum pname, ALfloat Data1, ALfloat Data2, float Proc);
    void CreateEffectSlots();
    void SetEffect();

    ALboolean createFilter(ALuint* filter, ALenum filterType, ALfloat gain, ALfloat gainHF);
    ALboolean setEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES* pEFXEAXReverb, ALuint uiEffect);
    ALboolean setEFXEAXReverbPropertiesAdvBlend(EFXEAXREVERBPROPERTIES* Imput1, EFXEAXREVERBPROPERTIES* Imput2, float Proc, ALuint uiEffect);
    ALboolean setEFXEAXReverbPropertiesSmooth(EFXEAXREVERBPROPERTIES* pEFXEAXReverb, ALuint uiEffect);

    ALboolean ALFWIsEFXSupported();
    ALboolean InitOpenAL();
    ALboolean ShutdownOpenAL();

    void setListenerOrientation(float fx, float fy, float fz, float ux, float uy, float uz);
    void setListenerPosition(float x, float y, float z, float rvrbpwr);
    void moveSound(ALuint Source, int x, int y);
    void stopSound(ALuint Source);
    void playSoundSpatial(int BUFFERNUMBER, ALfloat x, ALfloat y, float power, float pitch, int spatialAM);
    void playSound(int BUFFERNUMBER, ALfloat x, ALfloat y, float power, float pitch);
    ALuint playSoundAdvanced(int BUFFERNUMBER, ALfloat x, ALfloat y, float power, float pitch, bool loop);

    void loadSoundToBuffer(char* fname, int BUFFERNUMBER);
    ALboolean ALFWLoadWaveToBuffer(const char* szWaveFile, ALuint uiBufferID);

    bool isInitialized;


    CWaves* g_pWaveLoader = NULL;
    //ReverbController *ReverbEffect = NULL;

    ALuint Sounds_Bufer[MAXBUFERS];

    vector <ALuint> temporarySources;
    int MAXSources = 0;
    int PIXELSPERMETER = 12;//23;
    float ReverbPower = 0.4500000f;//0-1
    float RoomSizeCorrection = 1;
    float RoomSizeChangeTimer = 1;
    float LastRoomSize = 0;
    //SoundEmiter Testsound;


    LPALGENEFFECTS alGenEffects = NULL;
    LPALDELETEEFFECTS alDeleteEffects = NULL;
    LPALISEFFECT alIsEffect = NULL;
    LPALEFFECTI alEffecti = NULL;
    LPALEFFECTIV alEffectiv = NULL;
    LPALEFFECTF alEffectf = NULL;
    LPALEFFECTFV alEffectfv = NULL;
    LPALGETEFFECTI alGetEffecti = NULL;
    LPALGETEFFECTIV alGetEffectiv = NULL;
    LPALGETEFFECTF alGetEffectf = NULL;
    LPALGETEFFECTFV alGetEffectfv = NULL;

    //Filter objects
    LPALGENFILTERS alGenFilters = NULL;
    LPALDELETEFILTERS alDeleteFilters = NULL;
    LPALISFILTER alIsFilter = NULL;
    LPALFILTERI alFilteri = NULL;
    LPALFILTERIV alFilteriv = NULL;
    LPALFILTERF alFilterf = NULL;
    LPALFILTERFV alFilterfv = NULL;
    LPALGETFILTERI alGetFilteri = NULL;
    LPALGETFILTERIV alGetFilteriv = NULL;
    LPALGETFILTERF alGetFilterf = NULL;
    LPALGETFILTERFV alGetFilterfv = NULL;

    // Auxiliary slot object
    LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = NULL;
    LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = NULL;
    LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = NULL;
    LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = NULL;
    LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = NULL;
    LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = NULL;
    LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = NULL;
    LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = NULL;
    LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = NULL;
    LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = NULL;
    LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = NULL;

    // XRAM functions and enum values

    //LPEAXSETBUFFERMODE eaxSetBufferMode = NULL;
    //LPEAXGETBUFFERMODE eaxGetBufferMode = NULL;

    ALenum eXRAMSize = 0;
    ALenum eXRAMFree = 0;
    ALenum eXRAMAuto = 0;
    ALenum eXRAMHardware = 0;
    ALenum eXRAMAccessible = 0;

};

engineSound* M_getSound();
#endif 