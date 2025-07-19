
#include "al.h"
#include "alc.h"
#include "efx.h"
#include "efx-creative.h"
#include "EFX-Util.h"
#include "xram.h"
#include "engineSound.h"

#include "engine.h"

engineSound* localSound = NULL;
engineSound* M_getSound() { return localSound; }

engineSound::engineSound()
{
    localSound = this;
    M_getEngine()->registerModule(this, "Engine Sound");
}

engineSound::~engineSound() {

}


void engineSound::load() {

}

void engineSound::synchro() {

}

void engineSound::work() {

    debugMessage += "Im working!";

}

void engineSound::draw() {

}


ALboolean engineSound::InitOpenAL() {
	ALDeviceList* pDeviceList = NULL;
	ALCcontext* pContext = NULL;
	ALCdevice* pDevice = NULL;
	ALint i;
	ALboolean bReturn = AL_FALSE;
	//cout << "Searching for OpenAL Devices..." << endl;
	pDeviceList = new ALDeviceList();
	if ((pDeviceList) && (pDeviceList->GetNumDevices()))
	{
		//ALFWprintf("Detect OpenAL Devices: - - - - - -*\n");
		//for (i = 0; i < pDeviceList->GetNumDevices(); i++)
		//	ALFWprintf("'%d.  %s%s\n", i + 1, pDeviceList->GetDeviceName(i), i == pDeviceList->GetDefaultDevice() ? " - (default)" : "");

		i = 1;
		pDevice = alcOpenDevice(pDeviceList->GetDeviceName(i - 1));
		if (pDevice)
		{
			pContext = alcCreateContext(pDevice, NULL);
			if (pContext)
			{
				//ALFWprintf("\nUsing %s Device:\n", alcGetString(pDevice, ALC_DEVICE_SPECIFIER));
				alcMakeContextCurrent(pContext);
				bReturn = AL_TRUE;


				ALint		iSourcesGenerated;
				ALuint		uiSources[1024] = { 0 };
				for (iSourcesGenerated = 0; iSourcesGenerated < 1024; iSourcesGenerated++)
				{
					alGenSources(1, &uiSources[iSourcesGenerated]);
					if (alGetError() != AL_NO_ERROR)
						break;
				}
				for (int c = 0; c < iSourcesGenerated; c++)
					alDeleteSources(1, &uiSources[c]);
				MAXSources = iSourcesGenerated;
				//ALFWprintf("'%d Dynamic Sources Supported\n", iSourcesGenerated);

				ALenum eBufferFormat = 0;
				eBufferFormat = alGetEnumValue("AL_FORMAT_QUAD16");
				//printf("'4.0 playback support           %s\n", (eBufferFormat) ? "YES" : "NO");
				eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");
				//printf("'5.1 playback support           %s\n", (eBufferFormat) ? "YES" : "NO");
				eBufferFormat = alGetEnumValue("AL_FORMAT_61CHN16");
				//printf("'6.1 playback support           %s\n", (eBufferFormat) ? "YES" : "NO");
				eBufferFormat = alGetEnumValue("AL_FORMAT_71CHN16");
				//printf("'7.1 playback support           %s\n", (eBufferFormat) ? "YES" : "NO");
				if (ALFWIsEFXSupported())
				{
					//ALFWprintf("\nMaximum Effects Supported: - - - -*\n");

					ALuint		uiEffectSlots[128] = { 0 };
					ALuint		uiEffects[1] = { 0 };
					ALuint		uiFilters[1] = { 0 };
					ALint		iEffectSlotsGenerated;
					ALint		iSends;

					// To determine how many Auxiliary Effects Slots are available, create as many as possible (up to 128)
					// until the call fails.
					for (iEffectSlotsGenerated = 0; iEffectSlotsGenerated < 128; iEffectSlotsGenerated++)
					{
						alGenAuxiliaryEffectSlots(1, &uiEffectSlots[iEffectSlotsGenerated]);
						if (alGetError() != AL_NO_ERROR)
							break;
					}
					//ALFWprintf("%d Auxiliary Effect Slot%s\n", iEffectSlotsGenerated, (iEffectSlotsGenerated == 1) ? "" : "s");
					// Retrieve the number of Auxiliary Effect Slots Sends available on each Source
					alcGetIntegerv(pDevice, ALC_MAX_AUXILIARY_SENDS, 1, &iSends);
					printf("%d Auxiliary Send%s per Source\n", iSends, (iSends == 1) ? "" : "s");
					// To determine which Effects are supported, generate an Effect Object, and try to set its type to
					// the various Effect enum values
					printf("\nEffects Supported: - - - - - - - -*\n");
					alGenEffects(1, &uiEffects[0]);
					if (alGetError() == AL_NO_ERROR)
					{
						// Try setting Effect Type to known Effects
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_REVERB);
						printf("'Reverb' Support                %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
						printf("'EAX Reverb' Support            %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_CHORUS);
						printf("'Chorus' Support                %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_DISTORTION);
						printf("'Distortion' Support            %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_ECHO);
						printf("'Echo' Support                  %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
						printf("'Flanger' Support               %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_FREQUENCY_SHIFTER);
						printf("'Frequency Shifter' Support     %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_VOCAL_MORPHER);
						printf("'Vocal Morpher' Support         %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_PITCH_SHIFTER);
						printf("'Pitch Shifter' Support         %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_RING_MODULATOR);
						printf("'Ring Modulator' Support        %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_AUTOWAH);
						printf("'Autowah' Support               %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_COMPRESSOR);
						printf("'Compressor' Support            %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
						alEffecti(uiEffects[0], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);
						printf("'Equalizer' Support             %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
					}
					printf("\nFilters Supported: -\n");
					alGenFilters(1, &uiFilters[0]);
					if (alGetError() == AL_NO_ERROR)
					{
						// Try setting the Filter type to known Filters
						alFilteri(uiFilters[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
						printf("'Low Pass'  Support             %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");

						alFilteri(uiFilters[0], AL_FILTER_TYPE, AL_FILTER_HIGHPASS);
						printf("'High Pass' Support             %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");

						alFilteri(uiFilters[0], AL_FILTER_TYPE, AL_FILTER_BANDPASS);
						printf("'Band Pass' Support             %s\n", (alGetError() == AL_NO_ERROR) ? "YES" : "NO");
					}
					// Clean-up ...
					// Delete Filter
					alDeleteFilters(1, &uiFilters[0]);
					// Delete Effect
					alDeleteEffects(1, &uiEffects[0]);
					// Delete Auxiliary Effect Slots
					alDeleteAuxiliaryEffectSlots(iEffectSlotsGenerated, uiEffectSlots);
					//ALFWprintf("\nOpenAll Loaded - - - - - - - - - -*\n");

					alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);

					//ReverbEffect = new ReverbController();
					//ReverbEffect->CreateEffectSlots();
					//ReverbEffect->SetEffect();

					g_pWaveLoader = new CWaves();
				}
				else
				{
				}
					//ALFWprintf("Effects NOT Supported! -\n");
			}
			else
				alcCloseDevice(pDevice);
		}
		delete pDeviceList;
	}

	return bReturn;
}

ALboolean engineSound::ALFWIsEFXSupported(){

	ALCdevice* pDevice = NULL;
	ALCcontext* pContext = NULL;
	ALboolean bEFXSupport = AL_FALSE;

	pContext = alcGetCurrentContext();
	pDevice = alcGetContextsDevice(pContext);

	if (alcIsExtensionPresent(pDevice, (ALCchar*)ALC_EXT_EFX_NAME))
	{
		// Get function pointers
		this->alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		this->alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		this->alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
		this->alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
		this->alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
		this->alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
		this->alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
		this->alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
		this->alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
		this->alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
		this->alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
		this->alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
		this->alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
		this->alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
		this->alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
		this->alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
		this->alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
		this->alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
		this->alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
		this->alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
		this->alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
		this->alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
		this->alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
		this->alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
		this->alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
		this->alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
		this->alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
		this->alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
		this->alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
		this->alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
		this->alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
		this->alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
		this->alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");

		if (this->alGenEffects 
		&& this->alDeleteEffects 
		&& this->alIsEffect 
		&& this->alEffecti 
		&& this->alEffectiv 
		&& this->alEffectf 
		&& this->alEffectfv 
		&& this->alGetEffecti 
		&& this->alGetEffectiv 
		&& this->alGetEffectf 
		&& this->alGetEffectfv 
		&& this->alGenFilters 
		&& this->alDeleteFilters 
		&& this->alIsFilter 
		&& this->alFilteri 
		&& this->alFilteriv 
		&& this->alFilterf 
		&& this->alFilterfv 
		&& this->alGetFilteri 
		&& this->alGetFilteriv 
		&& this->alGetFilterf 
		&& this->alGetFilterfv 
		&& this->alGenAuxiliaryEffectSlots 
		&& this->alDeleteAuxiliaryEffectSlots 
		&& this->alIsAuxiliaryEffectSlot 
		&& this->alAuxiliaryEffectSloti 
		&& this->alAuxiliaryEffectSlotiv 
		&& this->alAuxiliaryEffectSlotf 
		&& this->alAuxiliaryEffectSlotfv 
		&& this->alGetAuxiliaryEffectSloti 
		&& this->alGetAuxiliaryEffectSlotiv 
		&& this->alGetAuxiliaryEffectSlotf 
		&& this->alGetAuxiliaryEffectSlotfv)
			bEFXSupport = AL_TRUE;
	}

	return bEFXSupport;
}






// Initialization and enumeration

/*void ALFWInit(){

}*//*
void ShutdownOpenAL(){
	if (g_pWaveLoader)
	{
		delete g_pWaveLoader;
		g_pWaveLoader = NULL;
	}
}*/
//EFXEAXREVERBPROPERTIES MixReverbProperties(EFXEAXREVERBPROPERTIES *AReverb, AReverb){


ALboolean engineSound::ShutdownOpenAL() {
	ALCcontext* pContext = NULL;
	ALCdevice* pDevice = NULL;

	pContext = alcGetCurrentContext();
	pDevice = alcGetContextsDevice(pContext);

	if (pContext) { cout << "Cleaning OpenAL Context..." << endl; alcMakeContextCurrent(NULL); alcDestroyContext(pContext); }
	if (pDevice) { cout << "Cleaning OpenAL Device..." << endl; alcCloseDevice(pDevice); }

	if (g_pWaveLoader)
	{
		cout << "Cleaning OpenAL WaveLoader..." << endl;
		delete g_pWaveLoader;
		g_pWaveLoader = NULL;
	}

	return AL_TRUE;
}
ALboolean engineSound::ALFWLoadWaveToBuffer(const char* szWaveFile, ALuint uiBufferID) {
	WAVEID			WaveID;
	ALint			iDataSize, iFrequency;
	ALenum			eBufferFormat;
	ALchar* pData;
	ALboolean		bReturn;

	bReturn = AL_FALSE;
	if (g_pWaveLoader)
	{
		if (SUCCEEDED(g_pWaveLoader->LoadWaveFile(szWaveFile, &WaveID)))
		{
			if ((SUCCEEDED(g_pWaveLoader->GetWaveSize(WaveID, (unsigned long*)&iDataSize))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveData(WaveID, (void**)&pData))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveFrequency(WaveID, (unsigned long*)&iFrequency))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveALBufferFormat(WaveID, &alGetEnumValue, (unsigned long*)&eBufferFormat))))
			{
				// Set XRAM Mode (if application)
				//if (eaxSetBufferMode && eXRAMBufferMode)
				//	eaxSetBufferMode(1, &uiBufferID, eXRAMBufferMode);

				alGetError();
				alBufferData(uiBufferID, eBufferFormat, pData, iDataSize, iFrequency);
				if (alGetError() == AL_NO_ERROR)
					bReturn = AL_TRUE;

				g_pWaveLoader->DeleteWaveFile(WaveID);
			}
		}
	}

	return bReturn;
}

ALchar fullPath[_MAX_PATH];
/*
ALchar* ALFWaddMediaPath(const ALchar* filename) {
	//sprintf(fullPath, "%s%s", "..\\..\\Media\\", filename);
	return fullPath;
}*/



ALboolean engineSound::setEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES* pEFXEAXReverb, ALuint uiEffect) {
	ALboolean bReturn = AL_FALSE;
	if (pEFXEAXReverb)
	{
		// Clear AL Error code
		alGetError();
		alEffectf(uiEffect, AL_EAXREVERB_DENSITY, pEFXEAXReverb->flDensity);
		alEffectf(uiEffect, AL_EAXREVERB_DIFFUSION, pEFXEAXReverb->flDiffusion);
		alEffectf(uiEffect, AL_EAXREVERB_GAIN, pEFXEAXReverb->flGain);
		alEffectf(uiEffect, AL_EAXREVERB_GAINHF, pEFXEAXReverb->flGainHF);
		alEffectf(uiEffect, AL_EAXREVERB_GAINLF, pEFXEAXReverb->flGainLF);
		alEffectf(uiEffect, AL_EAXREVERB_DECAY_TIME, pEFXEAXReverb->flDecayTime);
		alEffectf(uiEffect, AL_EAXREVERB_DECAY_HFRATIO, pEFXEAXReverb->flDecayHFRatio);
		alEffectf(uiEffect, AL_EAXREVERB_DECAY_LFRATIO, pEFXEAXReverb->flDecayLFRatio);
		alEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_GAIN, pEFXEAXReverb->flReflectionsGain);
		alEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_DELAY, pEFXEAXReverb->flReflectionsDelay);
		alEffectfv(uiEffect, AL_EAXREVERB_REFLECTIONS_PAN, pEFXEAXReverb->flReflectionsPan);
		alEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_GAIN, pEFXEAXReverb->flLateReverbGain);
		alEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_DELAY, pEFXEAXReverb->flLateReverbDelay);
		alEffectfv(uiEffect, AL_EAXREVERB_LATE_REVERB_PAN, pEFXEAXReverb->flLateReverbPan);
		alEffectf(uiEffect, AL_EAXREVERB_ECHO_TIME, pEFXEAXReverb->flEchoTime);
		alEffectf(uiEffect, AL_EAXREVERB_ECHO_DEPTH, pEFXEAXReverb->flEchoDepth);
		alEffectf(uiEffect, AL_EAXREVERB_MODULATION_TIME, pEFXEAXReverb->flModulationTime);
		alEffectf(uiEffect, AL_EAXREVERB_MODULATION_DEPTH, pEFXEAXReverb->flModulationDepth);
		alEffectf(uiEffect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, pEFXEAXReverb->flAirAbsorptionGainHF);
		alEffectf(uiEffect, AL_EAXREVERB_HFREFERENCE, pEFXEAXReverb->flHFReference);
		alEffectf(uiEffect, AL_EAXREVERB_LFREFERENCE, pEFXEAXReverb->flLFReference);
		alEffectf(uiEffect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, pEFXEAXReverb->flRoomRolloffFactor);
		alEffecti(uiEffect, AL_EAXREVERB_DECAY_HFLIMIT, pEFXEAXReverb->iDecayHFLimit);
		if (alGetError() == AL_NO_ERROR)
			bReturn = AL_TRUE;
	}

	return bReturn;
}

void engineSound::PropertieSmooth(ALuint uiEffect, ALenum pname, float Data) {
	//float smooth = 0.05f;
	ALfloat Var;
	alGetEffectf(uiEffect, pname, &Var);
	if (Var < Data) { Var += (Data - Var) * 0.50000f;  if (Var > Data) Var = Data; }
	if (Var > Data) { Var -= (Var - Data) * 0.50000f;  if (Var < Data) Var = Data; }
	alEffectf(uiEffect, pname, Var);
}

void engineSound::PropertieAdvBlend(ALuint uiEffect, ALenum pname, ALfloat Data1, ALfloat Data2, float Proc) {
	//float smooth = 0.05f;
   //Rpos = Position + ((float)TIME/(float)TIMEOUT)*(Target - Position);
	ALfloat Ret = Data1 * 1.00000f + Proc * (Data2 - Data1) * 1.00000f;
	// PropertieSmooth( uiEffect, pname , Ret);
	alEffectf(uiEffect, pname, Ret);
}
ALboolean engineSound::setEFXEAXReverbPropertiesAdvBlend(EFXEAXREVERBPROPERTIES* Imput1, EFXEAXREVERBPROPERTIES* Imput2, float Proc, ALuint uiEffect) {

	ALboolean bReturn = AL_FALSE;
	if (Imput1 && Imput2)
	{
		// Clear AL Error code
		alGetError();
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_DENSITY, Imput1->flDensity, Imput2->flDensity, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_DIFFUSION, Imput1->flDiffusion, Imput2->flDiffusion, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_GAIN, Imput1->flGain, Imput2->flGain, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_GAINHF, Imput1->flGainHF, Imput2->flGainHF, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_GAINLF, Imput1->flGainLF, Imput2->flGainLF, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_DECAY_TIME, Imput1->flDecayTime, Imput2->flDecayTime, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_DECAY_HFRATIO, Imput1->flDecayHFRatio, Imput2->flDecayHFRatio, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_DECAY_LFRATIO, Imput1->flDecayLFRatio, Imput2->flDecayLFRatio, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_REFLECTIONS_GAIN, Imput1->flReflectionsGain, Imput2->flReflectionsGain, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_REFLECTIONS_DELAY, Imput1->flReflectionsDelay, Imput2->flReflectionsDelay, Proc);
		//alEffectfv(uiEffect, AL_EAXREVERB_REFLECTIONS_PAN, Imput1->flReflectionsPan);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_LATE_REVERB_GAIN, Imput1->flLateReverbGain, Imput2->flLateReverbGain, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_LATE_REVERB_DELAY, Imput1->flLateReverbDelay, Imput2->flLateReverbDelay, Proc);
		//alEffectfv(uiEffect, AL_EAXREVERB_LATE_REVERB_PAN, Imput1->flLateReverbPan);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_ECHO_TIME, Imput1->flEchoTime, Imput2->flEchoTime, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_ECHO_DEPTH, Imput1->flEchoDepth, Imput2->flEchoDepth, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_MODULATION_TIME, Imput1->flModulationTime, Imput2->flModulationTime, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_MODULATION_DEPTH, Imput1->flModulationDepth, Imput2->flModulationDepth, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, Imput1->flAirAbsorptionGainHF, Imput2->flAirAbsorptionGainHF, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_HFREFERENCE, Imput1->flHFReference, Imput2->flHFReference, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_LFREFERENCE, Imput1->flLFReference, Imput2->flLFReference, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, Imput1->flRoomRolloffFactor, Imput2->flRoomRolloffFactor, Proc);
		this->PropertieAdvBlend(uiEffect, AL_EAXREVERB_DECAY_HFLIMIT, Imput1->iDecayHFLimit, Imput2->iDecayHFLimit, Proc);

		if (alGetError() == AL_NO_ERROR)
			bReturn = AL_TRUE;
	}

	return bReturn;


}


ALboolean engineSound::setEFXEAXReverbPropertiesSmooth(EFXEAXREVERBPROPERTIES* pEFXEAXReverb, ALuint uiEffect) {
	ALboolean bReturn = AL_FALSE;

	if (pEFXEAXReverb)
	{
		// Clear AL Error code
		alGetError();
		//float smooth = 0.05f;
		//ALfloat Var;
		   // alGetEffectf( uiEffect , AL_EAXREVERB_DENSITY , &Var );
		   // if (Var<pEFXEAXReverb->flDensity) {Var+=smooth; if(Var>pEFXEAXReverb->flDensity) Var=pEFXEAXReverb->flDensity; }
			//if (Var>pEFXEAXReverb->flDensity) {Var-=smooth; if(Var<pEFXEAXReverb->flDensity) Var=pEFXEAXReverb->flDensity; }
		PropertieSmooth(uiEffect, AL_EAXREVERB_DENSITY, pEFXEAXReverb->flDensity);
		PropertieSmooth(uiEffect, AL_EAXREVERB_DIFFUSION, pEFXEAXReverb->flDiffusion);
		PropertieSmooth(uiEffect, AL_EAXREVERB_GAIN, pEFXEAXReverb->flGain);
		PropertieSmooth(uiEffect, AL_EAXREVERB_GAINHF, pEFXEAXReverb->flGainHF);
		PropertieSmooth(uiEffect, AL_EAXREVERB_GAINLF, pEFXEAXReverb->flGainLF);
		PropertieSmooth(uiEffect, AL_EAXREVERB_DECAY_TIME, pEFXEAXReverb->flDecayTime);
		PropertieSmooth(uiEffect, AL_EAXREVERB_DECAY_HFRATIO, pEFXEAXReverb->flDecayHFRatio);
		PropertieSmooth(uiEffect, AL_EAXREVERB_DECAY_LFRATIO, pEFXEAXReverb->flDecayLFRatio);
		PropertieSmooth(uiEffect, AL_EAXREVERB_REFLECTIONS_GAIN, pEFXEAXReverb->flReflectionsGain);
		PropertieSmooth(uiEffect, AL_EAXREVERB_REFLECTIONS_DELAY, pEFXEAXReverb->flReflectionsDelay);
		alEffectfv(uiEffect, AL_EAXREVERB_REFLECTIONS_PAN, pEFXEAXReverb->flReflectionsPan);
		PropertieSmooth(uiEffect, AL_EAXREVERB_LATE_REVERB_GAIN, pEFXEAXReverb->flLateReverbGain);
		PropertieSmooth(uiEffect, AL_EAXREVERB_LATE_REVERB_DELAY, pEFXEAXReverb->flLateReverbDelay);
		alEffectfv(uiEffect, AL_EAXREVERB_LATE_REVERB_PAN, pEFXEAXReverb->flLateReverbPan);
		PropertieSmooth(uiEffect, AL_EAXREVERB_ECHO_TIME, pEFXEAXReverb->flEchoTime);
		PropertieSmooth(uiEffect, AL_EAXREVERB_ECHO_DEPTH, pEFXEAXReverb->flEchoDepth);
		PropertieSmooth(uiEffect, AL_EAXREVERB_MODULATION_TIME, pEFXEAXReverb->flModulationTime);
		PropertieSmooth(uiEffect, AL_EAXREVERB_MODULATION_DEPTH, pEFXEAXReverb->flModulationDepth);
		PropertieSmooth(uiEffect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, pEFXEAXReverb->flAirAbsorptionGainHF);
		PropertieSmooth(uiEffect, AL_EAXREVERB_HFREFERENCE, pEFXEAXReverb->flHFReference);
		PropertieSmooth(uiEffect, AL_EAXREVERB_LFREFERENCE, pEFXEAXReverb->flLFReference);
		PropertieSmooth(uiEffect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, pEFXEAXReverb->flRoomRolloffFactor);
		PropertieSmooth(uiEffect, AL_EAXREVERB_DECAY_HFLIMIT, pEFXEAXReverb->iDecayHFLimit);

		if (alGetError() == AL_NO_ERROR)
			bReturn = AL_TRUE;
	}

	return bReturn;
}

ALboolean engineSound::createFilter(ALuint* filter, ALenum filterType, ALfloat gain, ALfloat gainHF) {
	// Clear AL Error
	alGetError();

	alGenFilters(1, filter);
	if (alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	if (alIsFilter((*filter)))
	{
		alFilteri((*filter), AL_FILTER_TYPE, filterType);
		if (alGetError() != AL_NO_ERROR)
			return AL_FALSE;
	}

	alFilterf((*filter), AL_LOWPASS_GAIN, gain);
	alFilterf((*filter), AL_LOWPASS_GAINHF, gainHF);

	return AL_TRUE;
}

void engineSound::CreateEffectSlots() {

	/*AditionalReverbEnable = 0;

	ALFWprintf("\nMaking Reverb Controller: - - - - -*\n");
	alGenAuxiliaryEffectSlots(1, &uiEffectSlot);
	if (alGetError() == AL_NO_ERROR) ALFWprintf("'Created Reverb Effect Slot\n");
	else ALFWprintf("'Cannot Create Reverb Slot!\n");

	alGenEffects(1, &uiEffect);
	if (alGetError() == AL_NO_ERROR) ALFWprintf("'Created Reverb Effect\n");
	else ALFWprintf("'Cannot Create Reverb Effect!\n");

	alEffecti(*&uiEffect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
	if (alGetError() == AL_NO_ERROR) ALFWprintf("''Success!\n");
	else ALFWprintf("''Failed!!\n");



	ALFWprintf("\nMaking Aditional Reverb Controller: - - - - -*\n");
	alGenAuxiliaryEffectSlots(1, &uiAditEffectSlot);
	if (alGetError() == AL_NO_ERROR) ALFWprintf("'Created Aditional Reverb Effect Slot\n");
	else ALFWprintf("'Cannot Create Aditional Reverb Slot!\n");

	alGenEffects(1, &uiAditEffect);
	if (alGetError() == AL_NO_ERROR) ALFWprintf("'Created Aditional Reverb Effect\n");
	else ALFWprintf("'Cannot Create Aditional Reverb Effect!\n");

	alEffecti(*&uiAditEffect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
	if (alGetError() == AL_NO_ERROR) { ALFWprintf("''Success!\n"); AditionalReverbEnable = 1; }
	else ALFWprintf("''Failed!!\n");*/

}
void engineSound::SetEffect() {
/*	EAXREVERBPROPERTIES eaxReverb = REVERB_PRESET_SPORT_STADIUMTANNOY;
	EFXEAXREVERBPROPERTIES efxReverb;
	ConvertReverbParameters(&eaxReverb, &efxReverb);
	SetEFXEAXReverbProperties(&efxReverb, uiEffect);
	alAuxiliaryEffectSloti(uiEffectSlot, AL_EFFECTSLOT_EFFECT, uiEffect);*/

}

/*
void SetListenerRoomSize(float Size){
	int SoundBlendMode = 2;

	Size = max(0.0f,Size-60.0f);
   // if ( floor(Size*0.5)!=floor(LastRoomSize*0.5) )
   // {
		if (RoomSizeChangeTimer>0) RoomSizeChangeTimer-=0.75;
	 //   LastRoomSize = Size;
   // }

  //cout<<Size<<" ";
	if ( RoomSizeChangeTimer<=0)
//    if (ReverbEffect)
		if (ReverbEffect->AditionalReverbEnable)
		{
		RoomSizeChangeTimer += 1;
		ReverbEffect->SetEffect(); //
		EAXREVERBPROPERTIES eaxReverb[5] =
		{REVERB_PRESET_SPACESTATION_CUPBOARD
		,REVERB_PRESET_SPACESTATION_SMALLROOM
		,REVERB_PRESET_SPACESTATION_MEDIUMROOM
		,REVERB_PRESET_SPACESTATION_LARGEROOM
		,REVERB_PRESET_SPACESTATION_HALL};

		/*EAXREVERBPROPERTIES eaxReverb[5] =
		{REVERB_PRESET_FACTORY_ALCOVE
		,REVERB_PRESET_FACTORY_SHORTPASSAGE
		,REVERB_PRESET_FACTORY_MEDIUMROOM
		,REVERB_PRESET_FACTORY_LARGEROOM
		,REVERB_PRESET_FACTORY_HALL};

		int GearSize = 50; //if (SoundBlendMode = 2) GearSize = 90;
		int Gear = max(0,min(4,(int)(Size/GearSize)));
		EFXEAXREVERBPROPERTIES efxReverb;
		ConvertReverbParameters(&eaxReverb[Gear], &efxReverb);
			switch(SoundBlendMode)
			{
				case 0: // Easy , just select reverb  properties by room size and set it;
				SetEFXEAXReverbProperties(&efxReverb, ReverbEffect->uiAditEffect);
				///RoomSizeChangeTimer-=0.5; // Faster!
				break;
				case 1: // Medium, select properties and smoothly set it;
				SetEFXEAXReverbPropertiesSmooth(&efxReverb, ReverbEffect->uiAditEffect);
				//RoomSizeChangeTimer-=0.1;
				break;
				case 2: // Advanced, select 2 properties, and blended it by room size;
				EFXEAXREVERBPROPERTIES Imput2;
				ConvertReverbParameters(&eaxReverb[min(4,Gear+1)], &Imput2);
				float Proc = (Size- (Gear*GearSize) ) /((Gear+1)*GearSize);
				//cout<<Proc*100<<"% - ["<<(Gear)*GearSize <<" / "<< Size- (Gear*GearSize) <<"/"<< (Gear+1)*GearSize<<endl;
				SetEFXEAXReverbPropertiesAdvBlend(&efxReverb,&Imput2,Proc*1.00000f,ReverbEffect->uiAditEffect);
				//RoomSizeChangeTimer+=0.2; // Turbo !
				break;
			}


		alAuxiliaryEffectSlotf(ReverbEffect->uiAditEffectSlot, AL_EFFECTSLOT_GAIN ,  0.0f  );//
		for( int i = 0; i < (int)Temporary_Sources.size(); i++ )
		{
			ALint iState;
			ALfloat iGain;
			ALfloat iMGain;
			ALfloat iLGain;

			if (alIsSource( Temporary_Sources[i]  ))
			{
					alGetSourcei( Temporary_Sources[i] , AL_SOURCE_STATE, &iState);
					if (iState == AL_PLAYING  )
						{

								alGetSourcef( Temporary_Sources[i] , AL_GAIN, &iGain);
								alGetSourcef( Temporary_Sources[i] , AL_MIN_GAIN, &iLGain);
								alGetSourcef( Temporary_Sources[i] , AL_MAX_GAIN, &iMGain);
								alSourcef (Temporary_Sources[i], AL_GAIN, 0.0f );
								alSourcef (Temporary_Sources[i], AL_MIN_GAIN, 0.0f );
								alSourcef (Temporary_Sources[i], AL_MAX_GAIN, 0.0f );
								alSource3i( Temporary_Sources[i] , AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL , 2, AL_FILTER_NULL);

								alSourcef (Temporary_Sources[i], AL_GAIN, iGain );
								alSourcef (Temporary_Sources[i], AL_MIN_GAIN, iLGain );
								alSourcef (Temporary_Sources[i], AL_MAX_GAIN, iMGain );
								alSource3i(Temporary_Sources[i], AL_AUXILIARY_SEND_FILTER, ReverbEffect->uiAditEffectSlot, 2, AL_FILTER_NULL);
						}
			}
		}
		//alAuxiliaryEffectSloti(ReverbEffect->uiAditEffectSlot, AL_EFFECTSLOT_EFFECT,  ReverbEffect->uiAditEffect);



		RoomSizeCorrection = 1.0f - min(0.65f, (float)max( 0.30f ,Size/720.0f) );// cout<<Size<<endl;
		if (ReverbEffect)
		{
			 // ALL TIME ONE EFFECT REVERB
			alAuxiliaryEffectSlotf(ReverbEffect->uiEffectSlot, AL_EFFECTSLOT_GAIN , ReverbPower*0.15* ( 1.0f - RoomSizeCorrection*0.1f )  ); //
			// ROOM EFFECT REVERB
			if (ReverbEffect->AditionalReverbEnable )
			alAuxiliaryEffectSlotf(ReverbEffect->uiAditEffectSlot, AL_EFFECTSLOT_GAIN ,  ReverbPower*0.70f  );//
	   }

		//alAuxiliaryEffectSlotf(ReverbEffect->uiEffectSlot, AL_EFFECTSLOT_GAIN ,  0.0f);
		 cout<<"Sound Reverb tick RSize:"<<Size<<" R1:"<<ReverbPower*0.95* ( 1.0f - RoomSizeCorrection*0.2f )<< " R2:"<< ReverbPower*0.550f << " RSC:"<< RoomSizeCorrection <<" RBP:"<<ReverbPower<<endl;
		// cout<<"Sound Reverb tick RSize:"<<Size<<" / Reverb Preset:"<< Gear+1 <<endl;

		}

	   // alSource3i(uiSource, AL_AUXILIARY_SEND_FILTER, ReverbEffect->uiEffectSlot, 0, AL_FILTER_NULL);
}*/
void engineSound::setListenerOrientation(float fx, float fy, float fz, float ux, float uy, float uz) {
	//set the orientation using an array of floats
	float vec[6];
	vec[0] = fx;
	vec[1] = fy;
	vec[2] = fz;
	vec[3] = ux;
	vec[4] = uy;
	vec[5] = uz;
	alListenerfv(AL_ORIENTATION, vec);

}
void engineSound::setListenerPosition(float x, float y, float z, float rvrbpwr) {
	/*    x = floor(x);
		y = floor(y);
		z = floor(z);
		*///set the position using 3 seperate floats
	alListener3f(AL_POSITION, x / PIXELSPERMETER, (y + 75) / PIXELSPERMETER, z / PIXELSPERMETER);
	//float dir = 90;//Point_direction(Vec2(0,0),Vec2(aCamBlobX,aCamBlobY));
	float vec[6];
	/*vec[0] = cos(dir* M_PI/180) * 10000;
	vec[1] = -sin(dir* M_PI/180) * 10000;
	vec[2] = 10000.0f;

	vec[3] = cos(dir* M_PI/180) * 10000;
	vec[4] = -sin(dir* M_PI/180) * 10000;
	vec[5] =  -10000.0f;*/

	vec[0] = 0;
	vec[1] = -10000.0f;
	vec[2] = 10000.0f;

	vec[3] = 0;
	vec[4] = -10000.0f;
	vec[5] = -10000.0f;

	alListenerfv(AL_ORIENTATION, vec);

	//cout<<"TEST:"; for (int a = 0 ;a < 6 ; a++) cout<<vec[a]<<" / "; cout<<"]"<<endl;

	ReverbPower = rvrbpwr * 1.000000;
	/*if (ReverbEffect)
	{
		alAuxiliaryEffectSlotf(ReverbEffect->uiEffectSlot, AL_EFFECTSLOT_GAIN ,  ReverbPower*0.45f );
		if (ReverbEffect->AditionalReverbEnable )
		alAuxiliaryEffectSlotf(ReverbEffect->uiAditEffectSlot, AL_EFFECTSLOT_GAIN ,  ReverbPower*0.70f   );
		//else
		//alAuxiliaryEffectSlotf(ReverbEffect->uiAditEffectSlot, AL_EFFECTSLOT_GAIN ,  0.0f );
	}*/
	//PIXELSPERMETER = 11;//10 -(int)(ReverbPower*2);// cout<<PIXELSPERMETER<<endl;
}
void engineSound::moveSound(ALuint Source, int x, int y) {
	if (Source != NULL)
		if (alIsSource(Source))
		{
			ALint iState;
			alGetSourcei(Source, AL_SOURCE_STATE, &iState);
			if (iState == AL_PLAYING)
				alSource3f(Source, AL_POSITION, x / PIXELSPERMETER, y / PIXELSPERMETER, -1.0f);
		}
}

void engineSound::stopSound(ALuint Source) {
	if (Source != NULL)
		if (alIsSource(Source))
		{
			alSourcef(Source, AL_GAIN, 0.0f);
			alDeleteSources(1, &Source);
		}
}
void engineSound::playSoundSpatial(int BUFFERNUMBER, ALfloat x, ALfloat y, float power, float pitch, int spatialAM) {
	playSound(BUFFERNUMBER, x, y, power, pitch);

	playSound(BUFFERNUMBER, x - spatialAM, y - spatialAM, power / 5.0f, pitch);
	playSound(BUFFERNUMBER, x - spatialAM, y + spatialAM, power / 5.0f, pitch);
	playSound(BUFFERNUMBER, x + spatialAM, y - spatialAM, power / 5.0f, pitch);
	playSound(BUFFERNUMBER, x + spatialAM, y + spatialAM, power / 5.0f, pitch);
}
void engineSound::playSound(int BUFFERNUMBER, ALfloat x, ALfloat y, float power, float pitch) {
	alGetError();
	//cout<<(int)Temporary_Sources.size()<<endl;
	/*for( int i = 0; i < (int)Temporary_Sources.size(); i++ )
	{
		if (alIsSource( Temporary_Sources[i]  ))
		{
				ALint iState;
				alGetSourcei( Temporary_Sources[i] , AL_SOURCE_STATE, &iState);
				if (iState != AL_PLAYING  )
					{
						alSourcef ( Temporary_Sources[i] , AL_GAIN,  0  );
						alDeleteSources(1, &Temporary_Sources[i]);
						Temporary_Sources.erase (Temporary_Sources.begin()+i);
					}

		}
	}*/
	//cout<<"SNDS "<<Temporary_Sources.size()<<endl;
	ALuint uiSource = NULL;
	if (temporarySources.size() < (MAXSources / 2)) alGenSources(1, &uiSource);
	if (uiSource)
		if (alIsSource(uiSource))
		{
			//cout<<uiSource<<endl;
			alSourcei(uiSource, AL_BUFFER, Sounds_Bufer[BUFFERNUMBER]);
			if (alGetError() == AL_NO_ERROR)
			{

				alSourcef(uiSource, AL_PITCH, pitch);       //Set the pitch of the source
				alSourcef(uiSource, AL_MIN_GAIN, 0.0f);	//Set the gain of the source
				alSource3f(uiSource, AL_POSITION, x / PIXELSPERMETER, y / PIXELSPERMETER, -0.4400f * power);

				//if (ReverbEffect && ReverbPower>0.2f){
			   // alSource3i(uiSource, AL_AUXILIARY_SEND_FILTER, ReverbEffect->uiEffectSlot, 1, AL_FILTER_NULL);
				//    if (ReverbEffect->AditionalReverbEnable  )
				 //   alSource3i(uiSource, AL_AUXILIARY_SEND_FILTER, ReverbEffect->uiAditEffectSlot, 2, AL_FILTER_NULL);

				//}
				alSourcef(uiSource, AL_GAIN, 0.0f);
				alSourcePlay(uiSource);

				if (power == 0)
				{
					alSourcef(uiSource, AL_GAIN, 1.1f * RoomSizeCorrection);
					//alSourcef (uiSource, AL_MIN_GAIN,     0.75f     );
					alSourcef(uiSource, AL_SOURCE_RELATIVE, true);
					alSource3f(uiSource, AL_POSITION, 0, 0, 0);
				}
				else
				{
					/* alSourcef (uiSource, AL_GAIN,  1.95000f+power*1.00000f );
					 alSourcef (uiSource, AL_MAX_DISTANCE, 1100.000f+power*400.000f    );
					 alSourcef (uiSource, AL_REFERENCE_DISTANCE,  2.5500f+power*1.2500f);
					 alSourcef (uiSource, AL_ROLLOFF_FACTOR,   1.9500f-power*0.12500f  );*/

					alSourcef(uiSource, AL_GAIN, (0.20f + power * 1.50000f) * RoomSizeCorrection);
					alSourcef(uiSource, AL_MAX_DISTANCE, 1.000f + power * 450.000f);
					alSourcef(uiSource, AL_REFERENCE_DISTANCE, 0.3f + power * 4.00000f);
					alSourcef(uiSource, AL_ROLLOFF_FACTOR, 3.6500f - power * 0.14750f);//

				}


				if (alGetError() == AL_NO_ERROR)
					temporarySources.push_back(uiSource);
				else alDeleteSources(1, &uiSource);
			}
			else alDeleteSources(1, &uiSource);
		}
		else alDeleteSources(1, &uiSource);
}
ALuint engineSound::playSoundAdvanced(int BUFFERNUMBER, ALfloat x, ALfloat y, float power, float pitch, bool loop) {

	alGetError();

	ALuint uiSource = NULL;
	alGenSources(1, &uiSource);

	if (alIsSource(uiSource))
	{
		//cout<<uiSource<<endl;
		alSourcei(uiSource, AL_BUFFER, Sounds_Bufer[BUFFERNUMBER]);
		if (alGetError() == AL_NO_ERROR)
		{
			alSourcei(uiSource, AL_LOOPING, loop);
			alSourcef(uiSource, AL_PITCH, pitch);       //Set the pitch of the source
			alSourcef(uiSource, AL_MIN_GAIN, 0.0f);	//Set the gain of the source
			alSource3f(uiSource, AL_POSITION, x / PIXELSPERMETER, y / PIXELSPERMETER, -0.4400f * power);

			// if (ReverbEffect && ReverbPower>0.2f){
			// alSource3i(uiSource, AL_AUXILIARY_SEND_FILTER, ReverbEffect->uiEffectSlot, 1, AL_FILTER_NULL);
			//     if (ReverbEffect->AditionalReverbEnable  )
			//     alSource3i(uiSource, AL_AUXILIARY_SEND_FILTER, ReverbEffect->uiAditEffectSlot, 2, AL_FILTER_NULL);
			// }

			if (power == 0)
			{
				alSourcef(uiSource, AL_GAIN, 0.79f);
				alSourcef(uiSource, AL_MIN_GAIN, 0.75f);
				alSourcef(uiSource, AL_SOURCE_RELATIVE, true);
				alSource3f(uiSource, AL_POSITION, 0, 0, 0);
			}
			else
			{
				/*

								 alSourcef (uiSource, AL_GAIN,  ( 0.20f + power*1.10000f )* RoomSizeCorrection );
								 alSourcef (uiSource, AL_MAX_DISTANCE, 1.000f+power*400.000f    );
								 alSourcef (uiSource, AL_REFERENCE_DISTANCE,  0.3f+power*4.00000f);
								 alSourcef (uiSource, AL_ROLLOFF_FACTOR,    1.7200f-power*0.03750f   );*/

				alSourcef(uiSource, AL_GAIN, (0.20f + power * 1.50000f) * RoomSizeCorrection);
				alSourcef(uiSource, AL_MAX_DISTANCE, 1.000f + power * 450.000f);
				alSourcef(uiSource, AL_REFERENCE_DISTANCE, 0.3f + power * 4.00000f);
				alSourcef(uiSource, AL_ROLLOFF_FACTOR, 3.6500f - power * 0.14750f);//

			}
			alSourcePlay(uiSource);

			if (alGetError() != AL_NO_ERROR)
			{
				alDeleteSources(1, &uiSource); uiSource = NULL;
			}
		}
		else { alDeleteSources(1, &uiSource); uiSource = NULL; }
	}
	else { alDeleteSources(1, &uiSource); uiSource = NULL; }
	return uiSource;

}

void engineSound::loadSoundToBuffer(char* fname, int BUFFERNUMBER) {

	alGenBuffers(1, &Sounds_Bufer[BUFFERNUMBER]);
	if (!ALFWLoadWaveToBuffer(fname, Sounds_Bufer[BUFFERNUMBER]))
	{
		//printf("Failed to load %s\n", fname);
		alDeleteBuffers(1, &Sounds_Bufer[BUFFERNUMBER]);
	}
}

