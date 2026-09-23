#include "al.h"
#include "Audio/alAudio.h"

#include "alAudioEngine.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;
extern alLibImpl* g_alLib;

alAudio::alAudio()
{
}

alAudio::~alAudio()
{
}

alAudioBufferInfo alAudio::GetDeviceFormat()
{
	return g_alLib->m_audioEngine->GetDeviceInfo();
}

